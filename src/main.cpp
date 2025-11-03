#include <Arduino.h>

#include "config.hpp"
#include "gps.hpp"
#include "kinematics.hpp"
#include "logger.hpp"
#include "motors.hpp"
#include "persist.hpp"
#include "power.hpp"
#include "safety.hpp"
#include "solar_calc.hpp"
#include "ui.hpp"
#include "vector_math.hpp"

namespace {

enum class State {
    BOOT,
    HOMING,
    GPS_WAIT,
    IDLE_MANUAL,
    TARGET_ACQUIRE,
    LOCK_TARGET,
    TRACKING,
    PARK,
    STOW,
    FAULT
};

motors::Mount mount;
gps::Receiver gps_receiver;
ui::InputManager input;
ui::Display display;
persist::Data persist_data;
State current_state = State::BOOT;
kinematics::Pose mirror_pose{};
kinematics::Calibration calibration{};
Vec3 target_vector{0.0, 1.0, 0.0};
unsigned long last_control_update = 0;
unsigned long last_lcd_update = 0;
unsigned long last_battery_check = 0;
solar::SunPosition sun_position{};
char status_message[20] = "";

kinematics::Pose park_pose{0.0, 0.0};
kinematics::Pose stow_pose{180.0, 0.0};

void transition(State state) {
    current_state = state;
    switch (state) {
    case State::BOOT: strcpy(status_message, "BOOT"); break;
    case State::HOMING: strcpy(status_message, "HOMING"); break;
    case State::GPS_WAIT: strcpy(status_message, "GPS WAIT"); break;
    case State::IDLE_MANUAL: strcpy(status_message, "MANUAL"); break;
    case State::TARGET_ACQUIRE: strcpy(status_message, "AIM"); break;
    case State::LOCK_TARGET: strcpy(status_message, "LOCK"); break;
    case State::TRACKING:
        strcpy(status_message, "TRACK");
        last_control_update = 0;
        break;
    case State::PARK: strcpy(status_message, "PARK"); break;
    case State::STOW: strcpy(status_message, "STOW"); break;
    case State::FAULT: strcpy(status_message, "FAULT"); break;
    }
}

void move_to_pose(const kinematics::Pose &pose) {
    auto limited = kinematics::constrain_pose(pose, 0.0, 180.0);
    mount.move_to(limited);
    mirror_pose = limited;
}

Vec3 compute_target_vector() {
    kinematics::Calibration cal = calibration;
    Vec3 normal = kinematics::normal_from_pose(mirror_pose, cal);
    Vec3 target = (normal * 2.0) - sun_position.sun_vector_enu;
    return target.normalized();
}

void update_sun() {
    auto fix = gps_receiver.fix();
    if (!fix.valid) {
        sun_position = solar::SunPosition{};
        return;
    }
    solar::Observer obs{fix.latitude, fix.longitude, fix.altitude};
    unsigned long epoch = fix.epoch_seconds ? fix.epoch_seconds : (millis() / 1000);
    sun_position = solar::calculate(obs, epoch);
}

void refresh_lcd(State state) {
    if (millis() - last_lcd_update < config::LCD_REFRESH_MS) return;
    last_lcd_update = millis();

    ui::StatusSnapshot snapshot;
    snapshot.mode_label = status_message;
    snapshot.sun_az_deg = sun_position.azimuth_deg;
    snapshot.sun_el_deg = sun_position.altitude_deg;
    snapshot.mirror_pose = mirror_pose;
    auto batt = power::status();
    snapshot.battery_voltage = batt.voltage;
    snapshot.battery_percent = batt.percent;
    snapshot.gps_info = gps_receiver.fix();
    snapshot.message = safety::fault_label();

    if (state == State::TARGET_ACQUIRE) {
        display.set_page(ui::Page::Aiming);
    } else if (state == State::TRACKING) {
        display.set_page(ui::Page::Status);
    }

    display.render(snapshot);
}

void handle_fault(const char *message) {
    logger::append(message);
    transition(State::FAULT);
}

void update_state_machine() {
    auto button = input.poll();
    unsigned long current_time = millis();

    if (safety::estop_triggered()) {
        handle_fault("E-STOP");
    }

    if (button == ui::ButtonEvent::Menu && current_state != State::FAULT) {
        ui::Page page = display.page();
        switch (page) {
        case ui::Page::Status: page = ui::Page::Aiming; break;
        case ui::Page::Aiming: page = ui::Page::Power; break;
        case ui::Page::Power: page = ui::Page::Faults; break;
        case ui::Page::Faults: page = ui::Page::Status; break;
        }
        display.set_page(page);
    }

    if (current_state != State::FAULT && (current_time - last_battery_check) > 5000) {
        safety::check_battery();
        last_battery_check = current_time;
        if (safety::in_fault()) {
            transition(State::FAULT);
        }
    }

    switch (current_state) {
    case State::BOOT:
        persist_data = persist::load();
        calibration = persist_data.calibration;
        target_vector = persist_data.target_vector.normalized();
        mount.begin();
        gps_receiver.begin(Serial3);
        input.begin();
        display.begin();
        safety::begin();
        display.set_page(ui::Page::Status);
        transition(State::HOMING);
        break;

    case State::HOMING:
        if (mount.home_all()) {
            mirror_pose = {0.0, 0.0};
            transition(State::GPS_WAIT);
        }
        break;

    case State::GPS_WAIT:
        if (gps_receiver.fresh_fix()) {
            transition(State::IDLE_MANUAL);
        }
        break;

    case State::IDLE_MANUAL:
        if (button == ui::ButtonEvent::TargetAcquire) {
            transition(State::TARGET_ACQUIRE);
        } else if (button == ui::ButtonEvent::Park) {
            transition(State::PARK);
        } else if (button == ui::ButtonEvent::SetTarget && gps_receiver.fresh_fix()) {
            transition(State::TRACKING);
        }
        break;

    case State::TARGET_ACQUIRE:
        if (button == ui::ButtonEvent::SetTarget) {
            update_sun();
            target_vector = compute_target_vector();
            persist_data.target_vector = target_vector;
            persist_data.calibration = calibration;
            persist::save(persist_data);
            transition(State::TRACKING);
            break;
        }
        if (button == ui::ButtonEvent::Park) {
            transition(State::PARK);
            break;
        }
        {
            double rate_az = input.joystick_rate_x() * config::AZ_MAX_RATE_DEG_PER_S;
            double rate_el = input.joystick_rate_y() * config::EL_MAX_RATE_DEG_PER_S;
            mirror_pose.azimuth_deg += rate_az * 0.1;
            mirror_pose.elevation_deg += rate_el * 0.1;
            move_to_pose(mirror_pose);
        }
        break;

    case State::TRACKING:
        if (button == ui::ButtonEvent::Park) {
            transition(State::PARK);
            break;
        }
        if (safety::in_fault()) {
            transition(State::FAULT);
            break;
        }
        if (last_control_update == 0 || current_time - last_control_update >= config::CONTROL_UPDATE_MS) {
            last_control_update = current_time;
            update_sun();
            if (sun_position.altitude_deg < -2.0) {
                transition(State::PARK);
                break;
            }
            Vec3 normal = (sun_position.sun_vector_enu + target_vector).normalized();
            auto desired_pose = kinematics::mirror_pose_from_normal(normal, calibration);
            move_to_pose(desired_pose);
        }
        break;

    case State::PARK:
        move_to_pose(park_pose);
        transition(State::STOW);
        break;

    case State::STOW:
        move_to_pose(stow_pose);
        transition(State::IDLE_MANUAL);
        break;

    case State::FAULT:
        if (button == ui::ButtonEvent::Menu) {
            safety::clear_fault();
            transition(State::IDLE_MANUAL);
        }
        break;
    }

    refresh_lcd(current_state);
}

} // namespace

void setup() {
    Serial.begin(115200);
    while (!Serial && millis() < 2000) {}
    transition(State::BOOT);
}

void loop() {
    gps_receiver.poll();
    update_state_machine();
}
