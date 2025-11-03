#pragma once

#include <Arduino.h>
#include "vector_math.hpp"

#include "config.hpp"

namespace power {

struct BatteryStatus {
    float voltage{0.0f};
    uint8_t percent{0};
    bool low{false};
    bool critical{false};
};

inline float read_voltage() {
    constexpr float R1 = 100000.0f; // voltage divider values (ohms)
    constexpr float R2 = 10000.0f;
    constexpr float ADC_REF = 3.3f;
    constexpr float ADC_MAX = 4095.0f;
    float raw = analogRead(config::BATTERY_SENSE_PIN);
    float v = raw * ADC_REF / ADC_MAX;
    return v * (R1 + R2) / R2;
}

inline BatteryStatus status() {
    BatteryStatus s;
    s.voltage = read_voltage();
    float pct = (s.voltage - config::BATTERY_EMPTY) / (config::BATTERY_FULL - config::BATTERY_EMPTY);
    pct = clamp(pct, 0.0f, 1.0f);
    s.percent = static_cast<uint8_t>(pct * 100.0f + 0.5f);
    s.critical = s.voltage <= config::BATTERY_CRITICAL;
    s.low = s.voltage <= config::BATTERY_EMPTY;
    return s;
}

inline void set_fan_duty(float duty) {
    duty = clamp(duty, 0.0f, 1.0f);
    analogWrite(config::FAN_PWM_PIN, static_cast<int>(duty * 255));
}

} // namespace power
