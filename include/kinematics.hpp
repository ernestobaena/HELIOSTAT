#pragma once

#include <Arduino.h>

#include "config.hpp"
#include "solar_calc.hpp"

namespace kinematics {

struct MirrorAngles {
  float azimuthMotorDeg;
  float elevationMotorDeg;
};

struct TrackingSolution {
  MirrorAngles targets;
  float aimErrorDeg;
};

// Convert the sun position into desired mirror motor angles. The solver applies
// calibration data and backlash compensation according to the tables defined in
// config.hpp.
TrackingSolution solveTracking(const solar::SunPosition &sunPosition,
                               bool approachingFromPositive);

// Utility to interpolate calibration/backlash tables.
float interpolateCalibration(float sunElevationDeg,
                             const config::CalibrationTable &table);

}  // namespace kinematics

