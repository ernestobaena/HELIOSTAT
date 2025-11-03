#pragma once

#include <Arduino.h>

namespace config {

// Pin assignments
constexpr uint8_t AZ_STEP_PIN = 6;
constexpr uint8_t AZ_DIR_PIN = 7;
constexpr uint8_t AZ_EN_PIN = 8;
constexpr uint8_t EL_STEP_PIN = 9;
constexpr uint8_t EL_DIR_PIN = 10;
constexpr uint8_t EL_EN_PIN = 11;

constexpr uint8_t AZ_HOME_SWITCH = 22;
constexpr uint8_t AZ_LIMIT_SWITCH = 23;
constexpr uint8_t EL_HOME_SWITCH = 24;
constexpr uint8_t EL_LIMIT_SWITCH = 25;

constexpr uint8_t BTN_TARGET_ACQUIRE = 26;
constexpr uint8_t BTN_SET_START = 27;
constexpr uint8_t BTN_PARK = 28;
constexpr uint8_t BTN_MENU = 29;

constexpr uint8_t JOYSTICK_X = A0;
constexpr uint8_t JOYSTICK_Y = A1;
constexpr uint8_t JOYSTICK_PUSH = 30;

constexpr uint8_t ESTOP_INPUT = 31;
constexpr uint8_t BATTERY_SENSE_PIN = A2;
constexpr uint8_t FAN_PWM_PIN = 5;

// Motion control parameters
constexpr double AZ_STEPS_PER_DEG = 200.0 * 16.0 / 360.0 * 360.0 / 1.8; // placeholder conversion
constexpr double EL_STEPS_PER_DEG = 200.0 * 16.0 / 360.0 * 360.0 / 1.8;
constexpr double AZ_MAX_RATE_DEG_PER_S = 4.0;
constexpr double EL_MAX_RATE_DEG_PER_S = 4.0;

constexpr double AZ_BACKLASH_POS = 0.05; // degrees
constexpr double AZ_BACKLASH_NEG = 0.07;
constexpr double EL_BACKLASH_POS = 0.04;
constexpr double EL_BACKLASH_NEG = 0.06;

// Timing
constexpr unsigned long CONTROL_UPDATE_MS = 120000UL; // two minutes
constexpr unsigned long GPS_FRESHNESS_MS = 2000UL;
constexpr unsigned long LCD_REFRESH_MS = 500UL;

// Power thresholds
constexpr float BATTERY_FULL = 13.8f;
constexpr float BATTERY_EMPTY = 11.6f;
constexpr float BATTERY_CRITICAL = 11.2f;

// GPS parameters
constexpr uint8_t MIN_SATELLITES = 4;
constexpr float MAX_HDOP = 2.5f;

// Persistence
constexpr uint32_t PERSIST_MAGIC = 0x48454C49; // "HELI"
constexpr size_t MAX_LOG_ENTRIES = 128;

} // namespace config
