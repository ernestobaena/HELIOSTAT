#pragma once

#include <Arduino.h>
#include <array>

namespace config {

constexpr uint32_t SERIAL_BAUD_RATE = 115200UL;

// Pin assignments for the azimuth and elevation motor drivers.
constexpr uint8_t PIN_AZIMUTH_PWM = 2;
constexpr uint8_t PIN_AZIMUTH_DIR = 3;
constexpr uint8_t PIN_AZIMUTH_EN = 4;

constexpr uint8_t PIN_ELEVATION_PWM = 5;
constexpr uint8_t PIN_ELEVATION_DIR = 6;
constexpr uint8_t PIN_ELEVATION_EN = 7;

constexpr uint8_t PIN_UI_BUTTON = 8;
constexpr uint8_t PIN_UI_ENCODER_A = 9;
constexpr uint8_t PIN_UI_ENCODER_B = 10;
constexpr uint8_t PIN_UI_STATUS_LED = 11;

constexpr uint8_t PIN_GPS_RX = 14;  // Serial1 on the GIGA R1
constexpr uint8_t PIN_GPS_TX = 13;

constexpr uint8_t PIN_POWER_SENSE = A0;
constexpr uint8_t PIN_SAFETY_ESTOP = 22;
constexpr uint8_t PIN_SAFETY_AZ_LIMIT = 23;
constexpr uint8_t PIN_SAFETY_EL_LIMIT = 24;
constexpr uint8_t PIN_WIND_SPEED = A1;
constexpr uint8_t PIN_TEMPERATURE = A2;

// Motor parameters, expressed in meaningful engineering units.
constexpr float AZIMUTH_STEPS_PER_DEGREE = 512.0f;
constexpr float ELEVATION_STEPS_PER_DEGREE = 512.0f;
constexpr float AZIMUTH_MAX_SPEED_DPS = 6.0f;  // degrees per second
constexpr float ELEVATION_MAX_SPEED_DPS = 4.0f;
constexpr float AZIMUTH_MAX_ACCEL_DPS2 = 12.0f;
constexpr float ELEVATION_MAX_ACCEL_DPS2 = 8.0f;

constexpr float MIRROR_FOCAL_LENGTH_MM = 500.0f;
constexpr float MIRROR_AXIS_OFFSET_MM = 120.0f;
constexpr float TARGET_TOWER_AZIMUTH_DEG = 180.0f;
constexpr float TARGET_TOWER_ELEVATION_DEG = 45.0f;

// Calibration breakpoints for azimuth and elevation mirror angles.
constexpr size_t CALIBRATION_TABLE_SIZE = 6;
using CalibrationTable = std::array<float, CALIBRATION_TABLE_SIZE>;

// Example calibration values representing encoder degrees for the motor
// at equally spaced solar elevation breakpoints.
constexpr CalibrationTable AZIMUTH_CAL_DEG = {0.0f, 10.0f, 25.0f, 50.0f, 75.0f, 90.0f};
constexpr CalibrationTable ELEVATION_CAL_DEG = {0.0f, 12.0f, 28.0f, 55.0f, 82.0f, 96.0f};

// Backlash compensation tables expressed in motor degrees. Index 0 maps to
// the calibration breakpoints defined above.
constexpr CalibrationTable AZIMUTH_BACKLASH_DEG = {0.1f, 0.12f, 0.14f, 0.18f, 0.21f, 0.25f};
constexpr CalibrationTable ELEVATION_BACKLASH_DEG = {0.08f, 0.09f, 0.11f, 0.15f, 0.18f, 0.20f};

// Operational thresholds and compile-time constants referenced by modules.
constexpr float POWER_MIN_OPERATING_VOLTAGE = 20.5f;
constexpr float POWER_CRITICAL_VOLTAGE = 18.0f;
constexpr float POWER_MAX_VOLTAGE = 28.0f;
constexpr float POWER_DIVIDER_RATIO = 0.0125f;
constexpr float POWER_FILTER_ALPHA = 0.1f;

constexpr float SAFETY_WIND_SPEED_LIMIT = 14.0f;  // m/s
constexpr float SAFETY_TEMPERATURE_LIMIT = 70.0f; // Celsius

constexpr uint32_t GPS_FIX_TIMEOUT_MS = 300000UL;
constexpr uint32_t TRACKING_UPDATE_INTERVAL_MS = 1000UL;
constexpr uint32_t HOUSEKEEPING_INTERVAL_MS = 2000UL;
constexpr uint32_t PARKING_INTERVAL_MS = 15000UL;

// Persistent storage layout identifiers.
constexpr uint16_t PERSIST_MAGIC = 0x4853; // 'HS'
constexpr uint8_t PERSIST_VERSION = 1;

// Logging configuration.
constexpr size_t LOGGER_MAX_ENTRIES = 64;

}  // namespace config

