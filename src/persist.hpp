#pragma once

#include <Arduino.h>
#include <EEPROM.h>
#include "kinematics.hpp"

#include "config.hpp"
#include "vector_math.hpp"

namespace persist {

struct Data {
    uint32_t magic{config::PERSIST_MAGIC};
    Vec3 target_vector{0.0, 1.0, 0.0};
    kinematics::Calibration calibration{};
    uint32_t crc{0};
};

inline uint32_t crc32(const uint8_t *data, size_t length) {
    uint32_t crc = 0xFFFFFFFFu;
    for (size_t i = 0; i < length; ++i) {
        crc ^= data[i];
        for (int j = 0; j < 8; ++j) {
            if (crc & 1) {
                crc = (crc >> 1) ^ 0xEDB88320u;
            } else {
                crc >>= 1;
            }
        }
    }
    return ~crc;
}

inline void ensure_storage() {
#if defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_RP2040) || defined(ARDUINO_ARCH_SAMD) || defined(ARDUINO_ARCH_STM32)
    static bool initialized = false;
    if (!initialized) {
        EEPROM.begin(sizeof(Data));
        initialized = true;
    }
#endif
}

inline Data load() {
    ensure_storage();
    Data data;
    EEPROM.get(0, data);
    uint32_t expected = data.crc;
    data.crc = 0;
    uint32_t computed = crc32(reinterpret_cast<const uint8_t *>(&data), sizeof(Data));
    if (data.magic != config::PERSIST_MAGIC || expected != computed) {
        data = Data{};
        data.crc = crc32(reinterpret_cast<const uint8_t *>(&data), sizeof(Data));
        EEPROM.put(0, data);
#if defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_RP2040) || defined(ARDUINO_ARCH_SAMD) || defined(ARDUINO_ARCH_STM32)
        EEPROM.commit();
#endif
    } else {
        data.crc = expected;
    }
    return data;
}

inline void save(Data &data) {
    ensure_storage();
    data.magic = config::PERSIST_MAGIC;
    data.crc = 0;
    data.crc = crc32(reinterpret_cast<const uint8_t *>(&data), sizeof(Data));
    EEPROM.put(0, data);
#if defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_RP2040) || defined(ARDUINO_ARCH_SAMD) || defined(ARDUINO_ARCH_STM32)
    EEPROM.commit();
#endif
}

} // namespace persist
