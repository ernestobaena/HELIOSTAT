"""Quick desktop simulation for solar vector and mirror pose."""

from datetime import datetime, timezone
import math


def deg2rad(deg: float) -> float:
    return deg * math.pi / 180.0


def rad2deg(rad: float) -> float:
    return rad * 180.0 / math.pi


def normalize(vec):
    norm = math.sqrt(sum(v * v for v in vec))
    return tuple(v / norm for v in vec)


def sun_position(lat_deg: float, lon_deg: float, epoch: datetime):
    jd = (epoch.timestamp() / 86400.0) + 2440587.5
    T = (jd - 2451545.0) / 36525.0
    L0 = (280.46646 + T * (36000.76983 + T * 0.0003032)) % 360.0
    M = (357.52911 + T * (35999.05029 - 0.0001537 * T)) % 360.0
    e = 0.016708634 - T * (0.000042037 + 0.0000001267 * T)
    C = (1.914602 - T * (0.004817 + 0.000014 * T)) * math.sin(deg2rad(M))
    C += (0.019993 - 0.000101 * T) * math.sin(2 * deg2rad(M))
    C += 0.000289 * math.sin(3 * deg2rad(M))
    true_long = L0 + C
    omega = 125.04 - 1934.136 * T
    lambda_sun = true_long - 0.00569 - 0.00478 * math.sin(deg2rad(omega))
    epsilon0 = 23.439291 - T * (0.0130042 + T * (0.00000016 - 0.000000504 * T))
    epsilon = epsilon0 + 0.00256 * math.cos(deg2rad(omega))
    y = math.tan(deg2rad(epsilon) / 2.0)
    y *= y
    sin2L0 = math.sin(2.0 * deg2rad(L0))
    sinM = math.sin(deg2rad(M))
    cos2L0 = math.cos(2.0 * deg2rad(L0))
    sin4L0 = math.sin(4.0 * deg2rad(L0))
    sin2M = math.sin(2.0 * deg2rad(M))
    Etime = y * sin2L0 - 2.0 * e * sinM + 4.0 * e * y * sinM * cos2L0 - 0.5 * y * y * sin4L0 - 1.25 * e * e * sin2M
    Etime = rad2deg(Etime) * 4.0
    time_offset = Etime + 4.0 * lon_deg
    true_solar_time = (epoch.hour * 60 + epoch.minute + epoch.second / 60 + time_offset) % 1440
    hour_angle = true_solar_time / 4.0 - 180.0
    decl = rad2deg(math.asin(math.sin(deg2rad(epsilon)) * math.sin(deg2rad(lambda_sun))))
    lat_rad = deg2rad(lat_deg)
    decl_rad = deg2rad(decl)
    ha_rad = deg2rad(hour_angle)
    cos_zenith = math.sin(lat_rad) * math.sin(decl_rad) + math.cos(lat_rad) * math.cos(decl_rad) * math.cos(ha_rad)
    zenith = math.acos(max(-1.0, min(1.0, cos_zenith)))
    altitude = 90.0 - rad2deg(zenith)
    azimuth = rad2deg(math.atan2(math.sin(ha_rad), math.cos(ha_rad) * math.sin(lat_rad) - math.tan(decl_rad) * math.cos(lat_rad)))
    azimuth = (azimuth + 180.0) % 360.0
    alt_rad = deg2rad(altitude)
    az_rad = deg2rad(azimuth)
    sun_vec = (
        math.cos(alt_rad) * math.sin(az_rad),
        math.cos(alt_rad) * math.cos(az_rad),
        math.sin(alt_rad),
    )
    return normalize(sun_vec), altitude, azimuth


def mirror_pose_from_vectors(sun_vec, target_vec):
    normal = normalize(tuple(s + t for s, t in zip(sun_vec, target_vec)))
    elevation = rad2deg(math.asin(max(-1.0, min(1.0, normal[2]))))
    azimuth = rad2deg(math.atan2(normal[0], normal[1])) % 360.0
    return azimuth, elevation


if __name__ == "__main__":
    location = (39.7392, -104.9903)  # Denver, CO
    epoch = datetime(2024, 6, 21, 19, 0, tzinfo=timezone.utc)
    sun_vec, alt, az = sun_position(location[0], location[1], epoch)
    target = normalize((0.0, 10.0, 3.0))
    mir_az, mir_el = mirror_pose_from_vectors(sun_vec, target)
    print(f"Sun altitude {alt:.2f}°, azimuth {az:.2f}°")
    print(f"Mirror pose az {mir_az:.2f}°, el {mir_el:.2f}°")
