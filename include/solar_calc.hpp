#pragma once

#include <Arduino.h>

namespace solar {

struct DateTime {
  int year;
  int month;
  int day;
  int hour;
  int minute;
  float second;
};

struct Environment {
  double latitudeDeg;
  double longitudeDeg;
  double timezoneHours;
};

struct SunPosition {
  float azimuthDeg;
  float elevationDeg;
  float declinationDeg;
  float hourAngleDeg;
};

// Compute the sun position from the provided timestamp and environment data.
bool computeSunPosition(const DateTime &dateTime,
                        const Environment &environment,
                        SunPosition &result);

// Estimate the optical efficiency penalty due to cosine losses for the current elevation.
float cosineEfficiency(const SunPosition &position);

}  // namespace solar

