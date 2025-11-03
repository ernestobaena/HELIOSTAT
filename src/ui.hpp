#pragma once

#include <cmath>
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

#include "config.hpp"
#include "kinematics.hpp"
#include "power.hpp"
#include "gps.hpp"
#include "logger.hpp"

namespace ui {

enum class Page { Status, Aiming, Power, Faults };

enum class ButtonEvent { None, TargetAcquire, SetTarget, Park, Menu, JoystickPress };

struct StatusSnapshot {
    const char *mode_label{"BOOT"};
    double sun_az_deg{0.0};
    double sun_el_deg{0.0};
    kinematics::Pose mirror_pose{};
    float battery_voltage{0.0f};
    uint8_t battery_percent{0};
    gps::FixInfo gps_info{};
    const char *message{""};
};

class InputManager {
  public:
    void begin() {
        pinMode(config::BTN_TARGET_ACQUIRE, INPUT_PULLUP);
        pinMode(config::BTN_SET_START, INPUT_PULLUP);
        pinMode(config::BTN_PARK, INPUT_PULLUP);
        pinMode(config::BTN_MENU, INPUT_PULLUP);
        pinMode(config::JOYSTICK_PUSH, INPUT_PULLUP);
    }

    ButtonEvent poll() {
        if (read_edge(config::BTN_TARGET_ACQUIRE)) return ButtonEvent::TargetAcquire;
        if (read_edge(config::BTN_SET_START)) return ButtonEvent::SetTarget;
        if (read_edge(config::BTN_PARK)) return ButtonEvent::Park;
        if (read_edge(config::BTN_MENU)) return ButtonEvent::Menu;
        if (read_edge(config::JOYSTICK_PUSH)) return ButtonEvent::JoystickPress;
        return ButtonEvent::None;
    }

    double joystick_rate_x() const {
        int16_t raw = analogRead(config::JOYSTICK_X);
        return map_joystick(raw);
    }

    double joystick_rate_y() const {
        int16_t raw = analogRead(config::JOYSTICK_Y);
        return map_joystick(raw);
    }

  private:
    bool read_edge(uint8_t pin) {
        bool value = digitalRead(pin) == LOW;
        bool edge = value && !latched_[pin];
        latched_[pin] = value;
        return edge;
    }

    double map_joystick(int16_t raw) const {
        constexpr double deadband = 0.08;
        double normalized = (raw - 2048.0) / 2048.0;
        if (fabs(normalized) < deadband) return 0.0;
        double scaled = (fabs(normalized) - deadband) / (1.0 - deadband);
        return (normalized > 0.0 ? 1.0 : -1.0) * scaled;
    }

    mutable bool latched_[64]{};
};

class Display {
  public:
    void begin() {
        lcd_.init();
        lcd_.backlight();
    }

    void set_page(Page page) {
        if (page != current_page_) {
            current_page_ = page;
            lcd_.clear();
        }
    }

    Page page() const { return current_page_; }

    void render(const StatusSnapshot &status) {
        switch (current_page_) {
        case Page::Status:
            draw_status(status);
            break;
        case Page::Aiming:
            draw_aiming(status);
            break;
        case Page::Power:
            draw_power(status);
            break;
        case Page::Faults:
            draw_faults(status);
            break;
        }
    }

  private:
    void draw_status(const StatusSnapshot &s) {
        lcd_.setCursor(0, 0);
        lcd_.print("Mode: ");
        lcd_.print(s.mode_label);
        lcd_.setCursor(0, 1);
        lcd_.print("Sun A/E: ");
        lcd_.print(s.sun_az_deg, 1);
        lcd_.print("/");
        lcd_.print(s.sun_el_deg, 1);
        lcd_.setCursor(0, 2);
        lcd_.print("Mir A/E: ");
        lcd_.print(s.mirror_pose.azimuth_deg, 1);
        lcd_.print("/");
        lcd_.print(s.mirror_pose.elevation_deg, 1);
        lcd_.setCursor(0, 3);
        lcd_.print(s.message);
    }

    void draw_aiming(const StatusSnapshot &s) {
        lcd_.setCursor(0, 0);
        lcd_.print("AIM TARGET");
        lcd_.setCursor(0, 1);
        lcd_.print("Joystick X/Y:");
        lcd_.setCursor(0, 2);
        lcd_.print("Az:");
        lcd_.print(s.mirror_pose.azimuth_deg, 2);
        lcd_.setCursor(0, 3);
        lcd_.print("El:");
        lcd_.print(s.mirror_pose.elevation_deg, 2);
    }

    void draw_power(const StatusSnapshot &s) {
        lcd_.setCursor(0, 0);
        lcd_.print("Battery: ");
        lcd_.print(s.battery_percent);
        lcd_.print("% ");
        lcd_.print(s.battery_voltage, 2);
        lcd_.print("V");
        lcd_.setCursor(0, 1);
        lcd_.print("Sats:");
        lcd_.print(s.gps_info.satellites);
        lcd_.print(" HDOP:");
        lcd_.print(s.gps_info.hdop, 1);
        lcd_.setCursor(0, 2);
        lcd_.print("Fix age: ");
        lcd_.print(s.gps_info.age_ms);
        lcd_.print("ms");
    }

    void draw_faults(const StatusSnapshot &s) {
        lcd_.setCursor(0, 0);
        lcd_.print("FAULT LOG");
        auto tail = logger::tail(2);
        for (size_t i = 0; i < tail.size() && i < 3; ++i) {
            lcd_.setCursor(0, i + 1);
            lcd_.print(tail[i]);
        }
    }

    LiquidCrystal_I2C lcd_{0x27, 20, 4};
    Page current_page_{Page::Status};
};

} // namespace ui
