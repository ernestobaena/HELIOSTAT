#include "persist.hpp"

#include <EEPROM.h>

namespace persist {

namespace {
constexpr int kStorageAddress = 0;

#if defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_ESP8266) || \
    defined(ARDUINO_ARCH_RP2040) || defined(ARDUINO_ARCH_SAMD) || \
    defined(ARDUINO_ARCH_STM32)
void ensureEepromInitialized() {
  static bool initialized = false;
  if (!initialized) {
    EEPROM.begin(sizeof(PersistentData));
    initialized = true;
  }
}
#else
void ensureEepromInitialized() {}
#endif

}  // namespace

void Storage::begin() {
  ensureEepromInitialized();
  load();
}

void Storage::setData(const PersistentData &data) {
  data_ = data;
  dirty_ = true;
}

void Storage::commit() {
  if (!dirty_) {
    return;
  }
  ensureEepromInitialized();
  EEPROM.put(kStorageAddress, data_);
#if defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_ESP8266) || \
    defined(ARDUINO_ARCH_RP2040) || defined(ARDUINO_ARCH_SAMD) || \
    defined(ARDUINO_ARCH_STM32)
  EEPROM.commit();
#endif
  dirty_ = false;
}

void Storage::load() {
  EEPROM.get(kStorageAddress, data_);
  if (data_.magic != config::PERSIST_MAGIC || data_.version != config::PERSIST_VERSION) {
    data_ = {};
    data_.magic = config::PERSIST_MAGIC;
    data_.version = config::PERSIST_VERSION;
    dirty_ = true;
  } else {
    dirty_ = false;
  }
}

}  // namespace persist

