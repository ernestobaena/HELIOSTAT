#include <Arduino.h>
#include <math.h>

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

namespace {

enum class SystemState {
  kBoot,
  kAwaitFix,
  kTracking,
  kParking,
  kParked,
  kFault,
};

motors::Axis gAzimuthAxis(config::PIN_AZIMUTH_PWM, config::PIN_AZIMUTH_DIR,
                          config::PIN_AZIMUTH_EN, config::AZIMUTH_STEPS_PER_DEGREE);
motors::Axis gElevationAxis(config::PIN_ELEVATION_PWM, config::PIN_ELEVATION_DIR,
                            config::PIN_ELEVATION_EN, config::ELEVATION_STEPS_PER_DEGREE);

ui::UserInterface gUi;
gps::Receiver gGps;
power::Monitor gPower;
safety::Manager gSafety;
persist::Storage gStorage;
logger::Logger gLogger;

SystemState gState = SystemState::kBoot;
kinematics::TrackingSolution gLastSolution = {};
solar::SunPosition gLastSunPosition = {};
unsigned long gLastTrackingUpdate = 0;
unsigned long gLastHousekeeping = 0;
bool gApproachingPositive = true;

persist::PersistentData gPersisted;

const char *stateName(SystemState state) {
  switch (state) {
    case SystemState::kBoot:
      return "BOOT";
    case SystemState::kAwaitFix:
      return "AWAIT_FIX";
    case SystemState::kTracking:
      return "TRACKING";
    case SystemState::kParking:
      return "PARKING";
    case SystemState::kParked:
      return "PARKED";
    case SystemState::kFault:
      return "FAULT";
  }
  return "UNKNOWN";
}

void enterState(SystemState newState) {
  if (gState == newState) {
    return;
  }
  gLogger.log(logger::Level::kInfo, "STATE", "%s -> %s", stateName(gState),
              stateName(newState));
  gState = newState;
  switch (gState) {
    case SystemState::kBoot:
      gUi.setMode(ui::Mode::kBoot);
      break;
    case SystemState::kAwaitFix:
      gUi.setMode(ui::Mode::kIdle);
      break;
    case SystemState::kTracking:
      gUi.setMode(ui::Mode::kTracking);
      break;
    case SystemState::kParking:
      gUi.setMode(ui::Mode::kParking);
      break;
    case SystemState::kParked:
      gUi.setMode(ui::Mode::kIdle);
      break;
    case SystemState::kFault:
      gUi.setMode(ui::Mode::kFault);
      gAzimuthAxis.disable();
      gElevationAxis.disable();
      break;
  }
}

bool shouldTrack(const power::Status &powerStatus, const safety::Status &safetyStatus) {
  if (powerStatus.critical) {
    return false;
  }
  if (!gGps.hasFix()) {
    return false;
  }
  if (safetyStatus.inhibited) {
    return false;
  }
  if (gLastSunPosition.elevationDeg < 0.0f) {
    return false;
  }
  return true;
}

void updateTracking() {
  if (millis() - gLastTrackingUpdate < config::TRACKING_UPDATE_INTERVAL_MS) {
    return;
  }
  gLastTrackingUpdate = millis();

  const gps::Fix &fix = gGps.fix();
  solar::Environment env;
  env.latitudeDeg = fix.latitudeDeg;
  env.longitudeDeg = fix.longitudeDeg;
  env.timezoneHours = round(fix.longitudeDeg / 15.0);

  solar::DateTime dt = fix.dateTime;
  if (!solar::computeSunPosition(dt, env, gLastSunPosition)) {
    gLogger.log(logger::Level::kWarn, "SOLAR", "Computation failed for timestamp");
    return;
  }

  gApproachingPositive =
      (gLastSolution.targets.azimuthMotorDeg >= gAzimuthAxis.positionDeg());
  gLastSolution = kinematics::solveTracking(gLastSunPosition, gApproachingPositive);

  motors::MotionCommand azCmd;
  azCmd.targetDeg = gLastSolution.targets.azimuthMotorDeg;
  azCmd.maxSpeedDps = config::AZIMUTH_MAX_SPEED_DPS;
  azCmd.maxAccelDps2 = config::AZIMUTH_MAX_ACCEL_DPS2;
  gAzimuthAxis.applyCommand(azCmd);

  motors::MotionCommand elCmd;
  elCmd.targetDeg = gLastSolution.targets.elevationMotorDeg;
  elCmd.maxSpeedDps = config::ELEVATION_MAX_SPEED_DPS;
  elCmd.maxAccelDps2 = config::ELEVATION_MAX_ACCEL_DPS2;
  gElevationAxis.applyCommand(elCmd);
}

void updateParking() {
  motors::MotionCommand azCmd;
  azCmd.targetDeg = gPersisted.azimuthHomeDeg;
  azCmd.maxSpeedDps = config::AZIMUTH_MAX_SPEED_DPS;
  azCmd.maxAccelDps2 = config::AZIMUTH_MAX_ACCEL_DPS2;
  gAzimuthAxis.applyCommand(azCmd);

  motors::MotionCommand elCmd;
  elCmd.targetDeg = gPersisted.elevationHomeDeg;
  elCmd.maxSpeedDps = config::ELEVATION_MAX_SPEED_DPS;
  elCmd.maxAccelDps2 = config::ELEVATION_MAX_ACCEL_DPS2;
  gElevationAxis.applyCommand(elCmd);

  if (gAzimuthAxis.isAtTarget() && gElevationAxis.isAtTarget()) {
    gLastSolution.aimErrorDeg = 0.0f;
    enterState(SystemState::kParked);
    gLogger.log(logger::Level::kInfo, "MOTION", "Parked at %.2f / %.2f",
                gAzimuthAxis.positionDeg(), gElevationAxis.positionDeg());
  }
}

void performHousekeeping(const power::Status &powerStatus,
                         const safety::Status &safetyStatus) {
  if (millis() - gLastHousekeeping < config::HOUSEKEEPING_INTERVAL_MS) {
    return;
  }
  gLastHousekeeping = millis();

  ui::Telemetry telemetry;
  telemetry.aimErrorDeg = gLastSolution.aimErrorDeg;
  telemetry.powerVoltage = powerStatus.voltage;
  telemetry.gpsValid = gGps.hasFix();
  telemetry.safetyInhibit = safetyStatus.inhibited;
  gUi.setTelemetry(telemetry);

  if (gState == SystemState::kParked) {
    persist::PersistentData data = gPersisted;
    data.azimuthHomeDeg = gAzimuthAxis.positionDeg();
    data.elevationHomeDeg = gElevationAxis.positionDeg();
    gStorage.setData(data);
    gStorage.commit();
    gPersisted = gStorage.data();
  }
}

}  // namespace

void setup() {
  Serial.begin(config::SERIAL_BAUD_RATE);
  delay(500);
  gLogger.begin(Serial);
  gLogger.log(logger::Level::kInfo, "SYS", "HELIOSTAT boot sequence");

  gUi.begin();
  gSafety.begin();
  gPower.begin();
  gStorage.begin();
  gPersisted = gStorage.data();

  gAzimuthAxis.begin();
  gElevationAxis.begin();
  gAzimuthAxis.enable();
  gElevationAxis.enable();

  Serial1.begin(9600);
  gGps.begin(Serial1);

  enterState(SystemState::kAwaitFix);
  gLogger.log(logger::Level::kInfo, "SYS", "Awaiting GPS fix");
}

void loop() {
  gGps.update();
  gPower.update();
  gSafety.update();
  gUi.update();

  gAzimuthAxis.update();
  gElevationAxis.update();

  const power::Status powerStatus = gPower.status();
  const safety::Status safetyStatus = gSafety.status();

  if (powerStatus.critical) {
    gLogger.log(logger::Level::kError, "POWER", "Critical voltage %.2f",
                powerStatus.voltage);
    enterState(SystemState::kFault);
  } else if (safetyStatus.emergencyStop) {
    gLogger.log(logger::Level::kError, "SAFETY", "Emergency stop engaged");
    enterState(SystemState::kFault);
  }

  switch (gState) {
    case SystemState::kBoot:
      enterState(SystemState::kAwaitFix);
      break;
    case SystemState::kAwaitFix:
      if (gGps.hasFix()) {
        gLogger.log(logger::Level::kInfo, "GPS", "Fix acquired lat=%.4f lon=%.4f",
                    gGps.fix().latitudeDeg, gGps.fix().longitudeDeg);
        enterState(SystemState::kTracking);
      }
      break;
    case SystemState::kTracking:
      if (!shouldTrack(powerStatus, safetyStatus)) {
        enterState(SystemState::kParking);
        break;
      }
      updateTracking();
      break;
    case SystemState::kParking:
      updateParking();
      break;
    case SystemState::kParked:
      if (shouldTrack(powerStatus, safetyStatus)) {
        enterState(SystemState::kTracking);
      }
      break;
    case SystemState::kFault:
      // Wait for operator intervention.
      break;
  }

  if (gState == SystemState::kTracking) {
    const unsigned long gpsAge = millis() - gGps.lastUpdateMs();
    if (gpsAge > config::GPS_FIX_TIMEOUT_MS) {
      gLogger.log(logger::Level::kWarn, "GPS", "Fix timed out (%lu ms)", gpsAge);
      enterState(SystemState::kParking);
    }
  }

  performHousekeeping(powerStatus, safetyStatus);

  const ui::ButtonEvent event = gUi.lastButtonEvent();
  if (event != ui::ButtonEvent::kNone) {
    switch (event) {
      case ui::ButtonEvent::kShortPress:
        if (gState == SystemState::kTracking) {
          enterState(SystemState::kParking);
        } else if (gState == SystemState::kParked) {
          if (shouldTrack(powerStatus, safetyStatus)) {
            enterState(SystemState::kTracking);
          }
        }
        break;
      case ui::ButtonEvent::kLongPress:
        if (gState == SystemState::kFault) {
          gAzimuthAxis.enable();
          gElevationAxis.enable();
          enterState(SystemState::kParking);
        }
        break;
      case ui::ButtonEvent::kNone:
        break;
    }
    gUi.clearButtonEvent();
  }
}

