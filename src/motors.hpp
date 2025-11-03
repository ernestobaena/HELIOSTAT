#pragma once

#include <Arduino.h>

#include "config.hpp"
#include "kinematics.hpp"
#include "vector_math.hpp"

namespace motors {

enum class AxisId { Azimuth, Elevation };

enum class AxisState { Idle, Homing, Moving };

typedef void (*FaultCallback)(AxisId axis, const char *message);

struct AxisConfig {
    uint8_t step_pin;
    uint8_t dir_pin;
    uint8_t en_pin;
    uint8_t home_switch_pin;
    uint8_t limit_switch_pin;
    double steps_per_degree;
    double backlash_positive;
    double backlash_negative;
};

struct AxisStatus {
    AxisState state{AxisState::Idle};
    double current_position_deg{0.0};
    double home_offset_deg{0.0};
    bool homed{false};
};

class StepperAxis {
  public:
    StepperAxis(const AxisConfig &cfg, AxisId id) : config_(cfg), id_(id) {}

    void begin() {
        pinMode(config_.step_pin, OUTPUT);
        pinMode(config_.dir_pin, OUTPUT);
        pinMode(config_.en_pin, OUTPUT);
        pinMode(config_.home_switch_pin, INPUT_PULLUP);
        pinMode(config_.limit_switch_pin, INPUT_PULLUP);
        disable();
    }

    void attach_fault_callback(FaultCallback cb) { fault_callback_ = cb; }

    void enable() { digitalWrite(config_.en_pin, LOW); }
    void disable() { digitalWrite(config_.en_pin, HIGH); }

    void set_position_deg(double pos) {
        status_.current_position_deg = pos;
    }

    double position_deg() const { return status_.current_position_deg; }

    bool home(double search_rate_deg_s = 5.0) {
        status_.state = AxisState::Homing;
        enable();
        unsigned long start = millis();
        const unsigned long timeout = 20000UL;

        while ((millis() - start) < timeout) {
            if (digitalRead(config_.home_switch_pin) == LOW) {
                status_.homed = true;
                status_.current_position_deg = status_.home_offset_deg;
                status_.state = AxisState::Idle;
                disable();
                return true;
            }
            step(-search_rate_deg_s * 0.1); // coarse stepping backwards toward home
            delay(10);
        }

        status_.state = AxisState::Idle;
        disable();
        if (fault_callback_) {
            fault_callback_(id_, "Home timeout");
        }
        return false;
    }

    void move_absolute(double target_deg, double max_rate_deg_s = 2.0) {
        enable();
        double delta = target_deg - status_.current_position_deg;
        double backlash = delta >= 0 ? config_.backlash_positive : -config_.backlash_negative;
        double compensated = target_deg + backlash;
        step_to(compensated, max_rate_deg_s);
        status_.current_position_deg = target_deg;
        disable();
    }

    const AxisStatus &status() const { return status_; }

  private:
    void step_to(double target_deg, double max_rate_deg_s) {
        double delta = target_deg - status_.current_position_deg;
        long steps = static_cast<long>(delta * config_.steps_per_degree);
        if (steps == 0) {
            return;
        }

        digitalWrite(config_.dir_pin, steps > 0 ? HIGH : LOW);
        steps = abs(steps);
        unsigned long us_per_step = static_cast<unsigned long>(1000000.0 / (max_rate_deg_s * config_.steps_per_degree));
        if (us_per_step < 200) {
            us_per_step = 200;
        }
        for (long i = 0; i < steps; ++i) {
            digitalWrite(config_.step_pin, HIGH);
            delayMicroseconds(us_per_step / 2);
            digitalWrite(config_.step_pin, LOW);
            delayMicroseconds(us_per_step / 2);
        }
    }

    void step(double delta_deg) {
        long steps = static_cast<long>(delta_deg * config_.steps_per_degree);
        if (steps == 0) {
            steps = (delta_deg > 0) ? 1 : -1;
        }
        digitalWrite(config_.dir_pin, steps > 0 ? HIGH : LOW);
        steps = abs(steps);
        for (long i = 0; i < steps; ++i) {
            digitalWrite(config_.step_pin, HIGH);
            delayMicroseconds(500);
            digitalWrite(config_.step_pin, LOW);
            delayMicroseconds(500);
        }
        status_.current_position_deg += delta_deg;
    }

    AxisConfig config_;
    AxisId id_;
    AxisStatus status_{};
    FaultCallback fault_callback_{nullptr};
};

struct Mount {
    StepperAxis azimuth;
    StepperAxis elevation;

    Mount()
        : azimuth(AxisConfig{config::AZ_STEP_PIN, config::AZ_DIR_PIN, config::AZ_EN_PIN,
                             config::AZ_HOME_SWITCH, config::AZ_LIMIT_SWITCH, config::AZ_STEPS_PER_DEG,
                             config::AZ_BACKLASH_POS, config::AZ_BACKLASH_NEG},
                  AxisId::Azimuth),
          elevation(AxisConfig{config::EL_STEP_PIN, config::EL_DIR_PIN, config::EL_EN_PIN,
                               config::EL_HOME_SWITCH, config::EL_LIMIT_SWITCH, config::EL_STEPS_PER_DEG,
                               config::EL_BACKLASH_POS, config::EL_BACKLASH_NEG},
                    AxisId::Elevation) {}

    void begin() {
        azimuth.begin();
        elevation.begin();
    }

    bool home_all() {
        bool az = azimuth.home();
        bool el = elevation.home();
        return az && el;
    }

    void move_to(const kinematics::Pose &pose) {
        azimuth.move_absolute(pose.azimuth_deg, config::AZ_MAX_RATE_DEG_PER_S);
        elevation.move_absolute(pose.elevation_deg, config::EL_MAX_RATE_DEG_PER_S);
    }
};

} // namespace motors
