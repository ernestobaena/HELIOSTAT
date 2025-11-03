#include "solar_calc.hpp"

#include <math.h>

namespace solar {
namespace {
constexpr double kDegToRad = PI / 180.0;
constexpr double kRadToDeg = 180.0 / PI;

int dayOfYear(const DateTime &dt) {
  static const int daysUntilMonth[] = {0,   31,  59,  90,  120, 151,
                                       181, 212, 243, 273, 304, 334};
  int doy = daysUntilMonth[dt.month - 1] + dt.day;
  bool leap = (dt.year % 4 == 0 && dt.year % 100 != 0) || (dt.year % 400 == 0);
  if (leap && dt.month > 2) {
    ++doy;
  }
  return doy;
}

bool validate(const DateTime &dt) {
  if (dt.month < 1 || dt.month > 12) return false;
  if (dt.day < 1 || dt.day > 31) return false;
  if (dt.hour < 0 || dt.hour > 23) return false;
  if (dt.minute < 0 || dt.minute > 59) return false;
  if (dt.second < 0.0f || dt.second >= 60.0f) return false;
  return true;
}

}  // namespace

bool computeSunPosition(const DateTime &dateTime,
                        const Environment &environment,
                        SunPosition &result) {
  if (!validate(dateTime)) {
    return false;
  }

  const double latitudeRad = environment.latitudeDeg * kDegToRad;
  const double longitude = environment.longitudeDeg;
  const double gamma =
      2.0 * PI / 365.0 * (static_cast<double>(dayOfYear(dateTime)) - 1 +
                          (static_cast<double>(dateTime.hour) - 12.0) / 24.0);

  const double eqTime =
      229.18 * (0.000075 + 0.001868 * cos(gamma) - 0.032077 * sin(gamma) -
                0.014615 * cos(2 * gamma) - 0.040849 * sin(2 * gamma));

  const double decl =
      0.006918 - 0.399912 * cos(gamma) + 0.070257 * sin(gamma) -
      0.006758 * cos(2 * gamma) + 0.000907 * sin(2 * gamma) -
      0.002697 * cos(3 * gamma) + 0.00148 * sin(3 * gamma);

  const double timeOffset = eqTime + 4.0 * longitude -
                            60.0 * environment.timezoneHours;
  const double trueSolarMinutes =
      dateTime.hour * 60.0 + dateTime.minute + dateTime.second / 60.0 +
      timeOffset;
  const double hourAngleDeg = trueSolarMinutes / 4.0 - 180.0;
  const double hourAngleRad = hourAngleDeg * kDegToRad;

  const double cosZenith = sin(latitudeRad) * sin(decl) +
                           cos(latitudeRad) * cos(decl) * cos(hourAngleRad);
  double zenith = acos(constrain(cosZenith, -1.0, 1.0));
  const double elevation = PI / 2.0 - zenith;

  double azimuth =
      atan2(-sin(hourAngleRad),
            tan(decl) * cos(latitudeRad) - sin(latitudeRad) * cos(hourAngleRad));
  azimuth += PI;
  if (azimuth < 0) {
    azimuth += 2 * PI;
  } else if (azimuth >= 2 * PI) {
    azimuth -= 2 * PI;
  }

  result.azimuthDeg = azimuth * kRadToDeg;
  result.elevationDeg = elevation * kRadToDeg;
  result.declinationDeg = decl * kRadToDeg;
  result.hourAngleDeg = hourAngleDeg;
  return true;
}

float cosineEfficiency(const SunPosition &position) {
  const float elevationRad = position.elevationDeg * static_cast<float>(kDegToRad);
  const float cosLoss = cosf(elevationRad);
  return constrain(cosLoss, 0.0f, 1.0f);
}

}  // namespace solar

