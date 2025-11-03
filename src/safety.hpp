#pragma once

#include <Arduino.h>

#include "config.hpp"
#include "power.hpp"
#include "logger.hpp"

namespace safety {

enum class FaultCode {
    None,
    EmergencyStop,
    LowBattery,
    MotorFault,
    LimitExceeded
};

struct Status {
    FaultCode active_fault{FaultCode::None};
    bool estop_latched{false};
};

inline Status &state() {
    static Status status;
    return status;
}

inline void begin() {
    pinMode(config::ESTOP_INPUT, INPUT_PULLUP);
}

inline void clear_fault() {
    state().active_fault = FaultCode::None;
}

inline bool estop_triggered() {
    bool estop = digitalRead(config::ESTOP_INPUT) == LOW;
    if (estop) {
        state().estop_latched = true;
        state().active_fault = FaultCode::EmergencyStop;
        logger::append("E-STOP");
    }
    return estop;
}

inline void check_battery() {
    auto batt = power::status();
    if (batt.critical) {
        state().active_fault = FaultCode::LowBattery;
        logger::append("LOW BATT");
    }
}

inline bool in_fault() {
    return state().active_fault != FaultCode::None;
}

inline const char *fault_label() {
    switch (state().active_fault) {
    case FaultCode::None: return "OK";
    case FaultCode::EmergencyStop: return "E-STOP";
    case FaultCode::LowBattery: return "LOW BATT";
    case FaultCode::MotorFault: return "MOTOR";
    case FaultCode::LimitExceeded: return "LIMIT";
    }
    return "";
}

} // namespace safety
