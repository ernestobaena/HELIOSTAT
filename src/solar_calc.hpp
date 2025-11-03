#pragma once

#include <Arduino.h>
#include <cmath>

#include "vector_math.hpp"

namespace solar {

struct SunPosition {
    Vec3 sun_vector_enu{}; // unit vector
    double altitude_deg{0.0};
    double azimuth_deg{0.0};
};

struct Observer {
    double latitude_deg{0.0};
    double longitude_deg{0.0};
    double elevation_m{0.0};
};

// Utility: compute Julian Day from Unix epoch seconds
inline double julian_day(unsigned long epoch_seconds) {
    constexpr double UNIX_EPOCH_JD = 2440587.5;
    return epoch_seconds / 86400.0 + UNIX_EPOCH_JD;
}

inline double normalize_angle(double deg) {
    double result = fmod(deg, 360.0);
    if (result < 0.0) result += 360.0;
    return result;
}

inline double equation_of_time(double L0, double e, double M, double epsilon) {
    double y = tan(deg2rad(epsilon) / 2.0);
    y *= y;
    double sin2L0 = sin(2.0 * deg2rad(L0));
    double sinM = sin(deg2rad(M));
    double cos2L0 = cos(2.0 * deg2rad(L0));
    double sin4L0 = sin(4.0 * deg2rad(L0));
    double sin2M = sin(2.0 * deg2rad(M));
    double Etime = y * sin2L0 - 2.0 * e * sinM + 4.0 * e * y * sinM * cos2L0 - 0.5 * y * y * sin4L0 - 1.25 * e * e * sin2M;
    return rad2deg(Etime) * 4.0; // minutes
}

inline SunPosition calculate(const Observer &observer, unsigned long epoch_seconds) {
    double jd = julian_day(epoch_seconds);
    double T = (jd - 2451545.0) / 36525.0;

    double L0 = normalize_angle(280.46646 + T * (36000.76983 + T * 0.0003032));
    double M = normalize_angle(357.52911 + T * (35999.05029 - 0.0001537 * T));
    double e = 0.016708634 - T * (0.000042037 + 0.0000001267 * T);

    double C = (1.914602 - T * (0.004817 + 0.000014 * T)) * sin(deg2rad(M))
             + (0.019993 - 0.000101 * T) * sin(2 * deg2rad(M))
             + 0.000289 * sin(3 * deg2rad(M));
    double true_long = L0 + C;
    double omega = 125.04 - 1934.136 * T;
    double lambda = true_long - 0.00569 - 0.00478 * sin(deg2rad(omega));
    double epsilon0 = 23.439291 - T * (0.0130042 + T * (0.00000016 - 0.000000504 * T));
    double epsilon = epsilon0 + 0.00256 * cos(deg2rad(omega));

    double Etime = equation_of_time(L0, e, M, epsilon);

    double time_offset = Etime + 4.0 * observer.longitude_deg;
    double true_solar_time_min = fmod(epoch_seconds / 60.0 + time_offset, 1440.0);

    double hour_angle = (true_solar_time_min / 4.0 < 0)
                            ? true_solar_time_min / 4.0 + 180.0
                            : true_solar_time_min / 4.0 - 180.0;

    double decl = rad2deg(asin(sin(deg2rad(epsilon)) * sin(deg2rad(lambda))));

    double lat_rad = deg2rad(observer.latitude_deg);
    double decl_rad = deg2rad(decl);
    double ha_rad = deg2rad(hour_angle);

    double cos_zenith = sin(lat_rad) * sin(decl_rad) + cos(lat_rad) * cos(decl_rad) * cos(ha_rad);
    double zenith_angle = acos(clamp(cos_zenith, -1.0, 1.0));
    double altitude = 90.0 - rad2deg(zenith_angle);

    double azimuth = rad2deg(atan2(sin(ha_rad), cos(ha_rad) * sin(lat_rad) - tan(decl_rad) * cos(lat_rad)));
    azimuth = normalize_angle(azimuth + 180.0); // convert from astronomical to navigation convention

    // Convert to ENU unit vector
    double alt_rad = deg2rad(altitude);
    double az_rad = deg2rad(azimuth);

    Vec3 sun_enu{
        cos(alt_rad) * sin(az_rad), // East
        cos(alt_rad) * cos(az_rad), // North
        sin(alt_rad)               // Up
    };

    return SunPosition{sun_enu.normalized(), altitude, azimuth};
}

} // namespace solar
