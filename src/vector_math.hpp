#pragma once

#include <cmath>
#include <array>

constexpr double PI = 3.14159265358979323846;

struct Vec3 {
    double x{0.0};
    double y{0.0};
    double z{0.0};

    constexpr Vec3() = default;
    constexpr Vec3(double x_, double y_, double z_) : x(x_), y(y_), z(z_) {}

    double norm() const {
        return std::sqrt(x * x + y * y + z * z);
    }

    Vec3 normalized() const {
        double n = norm();
        if (n <= 0.0) {
            return Vec3{0.0, 0.0, 0.0};
        }
        return (*this) / n;
    }

    Vec3 operator+(const Vec3 &other) const {
        return Vec3{x + other.x, y + other.y, z + other.z};
    }

    Vec3 operator-(const Vec3 &other) const {
        return Vec3{x - other.x, y - other.y, z - other.z};
    }

    Vec3 operator*(double scalar) const {
        return Vec3{x * scalar, y * scalar, z * scalar};
    }

    Vec3 operator/(double scalar) const {
        return Vec3{x / scalar, y / scalar, z / scalar};
    }

    Vec3 &operator+=(const Vec3 &other) {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }
};

inline double dot(const Vec3 &a, const Vec3 &b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline Vec3 cross(const Vec3 &a, const Vec3 &b) {
    return Vec3{a.y * b.z - a.z * b.y,
                a.z * b.x - a.x * b.z,
                a.x * b.y - a.y * b.x};
}

inline double clamp(double value, double min_value, double max_value) {
    if (value < min_value) return min_value;
    if (value > max_value) return max_value;
    return value;
}

inline double rad2deg(double rad) {
    return rad * 180.0 / PI;
}

inline double deg2rad(double deg) {
    return deg * PI / 180.0;
}
