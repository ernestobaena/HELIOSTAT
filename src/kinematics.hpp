#pragma once

#include <Arduino.h>

#include "config.hpp"
#include "vector_math.hpp"

namespace kinematics {

struct Pose {
    double azimuth_deg{0.0};
    double elevation_deg{0.0};
};

struct Calibration {
    double azimuth_offset_deg{0.0};
    double elevation_offset_deg{0.0};
};

inline Pose mirror_pose_from_normal(const Vec3 &normal, const Calibration &cal) {
    Vec3 n = normal.normalized();

    double elevation = rad2deg(asin(clamp(n.z, -1.0, 1.0)));
    double azimuth = rad2deg(atan2(n.x, n.y));
    if (azimuth < 0.0) {
        azimuth += 360.0;
    }

    azimuth += cal.azimuth_offset_deg;
    elevation += cal.elevation_offset_deg;

    return Pose{azimuth, elevation};
}

inline Vec3 normal_from_pose(const Pose &pose, const Calibration &cal) {
    double az = deg2rad(pose.azimuth_deg - cal.azimuth_offset_deg);
    double el = deg2rad(pose.elevation_deg - cal.elevation_offset_deg);
    Vec3 n{
        cos(el) * sin(az),
        cos(el) * cos(az),
        sin(el)
    };
    return n.normalized();
}

inline Pose constrain_pose(const Pose &pose, double min_elev = 0.0, double max_elev = 180.0) {
    double az = fmod(pose.azimuth_deg, 360.0);
    if (az < 0.0) az += 360.0;
    double el = clamp(pose.elevation_deg, min_elev, max_elev);
    return Pose{az, el};
}

inline Pose mirror_pose_from_vectors(const Vec3 &sun, const Vec3 &target, const Calibration &cal) {
    Vec3 normal = (sun.normalized() + target.normalized()).normalized();
    return mirror_pose_from_normal(normal, cal);
}

} // namespace kinematics
