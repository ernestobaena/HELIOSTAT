#pragma once

#include <TimeLib.h>
#include <Arduino.h>
#include <TinyGPSPlus.h>

#include "config.hpp"
#include "solar_calc.hpp"

namespace gps {

struct FixInfo {
    double latitude{0.0};
    double longitude{0.0};
    double altitude{0.0};
    uint8_t satellites{0};
    float hdop{99.0f};
    unsigned long age_ms{UINT32_MAX};
    bool valid{false};
    uint32_t epoch_seconds{0};
};

class Receiver {
  public:
    void begin(HardwareSerial &serial) {
        serial_ = &serial;
        serial.begin(9600);
    }

    void poll() {
        if (!serial_) return;
        while (serial_->available() > 0) {
            gps_.encode(serial_->read());
        }
    }

    FixInfo fix() const {
        FixInfo info;
        if (gps_.location.isValid()) {
            info.latitude = gps_.location.lat();
            info.longitude = gps_.location.lng();
            info.valid = true;
        }
        if (gps_.altitude.isValid()) {
            info.altitude = gps_.altitude.meters();
        }
        if (gps_.satellites.isValid()) {
            info.satellites = gps_.satellites.value();
        }
        if (gps_.hdop.isValid()) {
            info.hdop = gps_.hdop.hdop();
        }
        info.age_ms = gps_.location.age();
        if (gps_.date.isValid() && gps_.time.isValid()) {
            info.epoch_seconds = to_epoch_seconds();
        }
        return info;
    }

    bool fresh_fix() const {
        FixInfo info = fix();
        return info.valid && info.satellites >= config::MIN_SATELLITES && info.hdop <= config::MAX_HDOP && info.age_ms <= config::GPS_FRESHNESS_MS;
    }

    solar::Observer observer() const {
        FixInfo info = fix();
        solar::Observer obs;
        obs.latitude_deg = info.latitude;
        obs.longitude_deg = info.longitude;
        obs.elevation_m = info.altitude;
        return obs;
    }

    uint32_t to_epoch_seconds() const {
        if (!gps_.date.isValid() || !gps_.time.isValid()) {
            return millis() / 1000;
        }
        tmElements_t tm;
        tm.Year = gps_.date.year() - 1970;
        tm.Month = gps_.date.month();
        tm.Day = gps_.date.day();
        tm.Hour = gps_.time.hour();
        tm.Minute = gps_.time.minute();
        tm.Second = gps_.time.second();
        return makeTime(tm);
    }

  private:
    TinyGPSPlus gps_;
    HardwareSerial *serial_{nullptr};
};

} // namespace gps
