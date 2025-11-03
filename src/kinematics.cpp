#include "kinematics.hpp"

#include <math.h>

namespace kinematics {
namespace {
struct Vec3 {
  double x;
  double y;
  double z;
};

constexpr double kDegToRad = PI / 180.0;
constexpr double kRadToDeg = 180.0 / PI;

Vec3 fromAzEl(double azDeg, double elDeg) {
  const double azRad = azDeg * kDegToRad;
  const double elRad = elDeg * kDegToRad;
  const double cosEl = cos(elRad);
  return {cosEl * sin(azRad), cosEl * cos(azRad), sin(elRad)};
}

Vec3 normalize(Vec3 v) {
  const double mag = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
  if (mag < 1e-6) {
    return {0.0, 0.0, 1.0};
  }
  return {v.x / mag, v.y / mag, v.z / mag};
}

double dot(Vec3 a, Vec3 b) { return a.x * b.x + a.y * b.y + a.z * b.z; }

Vec3 reflect(Vec3 incident, Vec3 normal) {
  const double ndoti = dot(normal, incident);
  return {incident.x - 2 * ndoti * normal.x,
          incident.y - 2 * ndoti * normal.y,
          incident.z - 2 * ndoti * normal.z};
}

float toAzimuth(Vec3 v) {
  float az = static_cast<float>(atan2(v.x, v.y) * kRadToDeg);
  if (az < 0.0f) {
    az += 360.0f;
  }
  return az;
}

float toElevation(Vec3 v) {
  return static_cast<float>(asin(constrain(v.z, -1.0, 1.0)) * kRadToDeg);
}

float interpolate(const config::CalibrationTable &table, float fraction) {
  if (fraction <= 0.0f) {
    return table.front();
  }
  if (fraction >= 1.0f) {
    return table.back();
  }
  const float scaled = fraction * (table.size() - 1);
  const size_t index = static_cast<size_t>(scaled);
  const float frac = scaled - index;
  const float a = table[index];
  const float b = table[index + 1];
  return a + (b - a) * frac;
}

}  // namespace

TrackingSolution solveTracking(const solar::SunPosition &sunPosition,
                               bool approachingFromPositive) {
  const Vec3 sunVec = normalize(fromAzEl(sunPosition.azimuthDeg, sunPosition.elevationDeg));
  const Vec3 towerVec = normalize(fromAzEl(config::TARGET_TOWER_AZIMUTH_DEG,
                                           config::TARGET_TOWER_ELEVATION_DEG));
  Vec3 normal = {sunVec.x + towerVec.x, sunVec.y + towerVec.y, sunVec.z + towerVec.z};
  normal = normalize(normal);

  const float normalAz = toAzimuth(normal);
  const float normalEl = toElevation(normal);

  const float minElevation = config::ELEVATION_CAL_DEG.front();
  const float maxElevation = config::ELEVATION_CAL_DEG.back();
  float fraction = 0.0f;
  if (maxElevation > minElevation) {
    fraction = constrain((sunPosition.elevationDeg - minElevation) /
                             (maxElevation - minElevation),
                         0.0f, 1.0f);
  }

  const float azCalibration = interpolate(config::AZIMUTH_CAL_DEG, fraction);
  const float elCalibration = interpolate(config::ELEVATION_CAL_DEG, fraction);
  const float azBacklash = interpolate(config::AZIMUTH_BACKLASH_DEG, fraction);
  const float elBacklash = interpolate(config::ELEVATION_BACKLASH_DEG, fraction);

  const float backlashSign = approachingFromPositive ? 1.0f : -1.0f;
  TrackingSolution solution;
  solution.targets.azimuthMotorDeg = normalAz + azCalibration + backlashSign * azBacklash;
  solution.targets.elevationMotorDeg = normalEl + elCalibration + backlashSign * elBacklash;

  const Vec3 reflected = normalize(reflect(sunVec, normal));
  const double errorCos = constrain(dot(reflected, towerVec), -1.0, 1.0);
  solution.aimErrorDeg = static_cast<float>(acos(errorCos) * kRadToDeg);
  return solution;
}

float interpolateCalibration(float sunElevationDeg,
                             const config::CalibrationTable &table) {
  const float minElevation = config::ELEVATION_CAL_DEG.front();
  const float maxElevation = config::ELEVATION_CAL_DEG.back();
  float fraction = 0.0f;
  if (maxElevation > minElevation) {
    fraction = constrain((sunElevationDeg - minElevation) /
                             (maxElevation - minElevation),
                         0.0f, 1.0f);
  }
  return interpolate(table, fraction);
}

}  // namespace kinematics

