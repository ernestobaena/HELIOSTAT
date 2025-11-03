#pragma once

#include <Arduino.h>

#include "config.hpp"

namespace persist {

struct PersistentData {
  uint16_t magic = config::PERSIST_MAGIC;
  uint8_t version = config::PERSIST_VERSION;
  float azimuthHomeDeg = 0.0f;
  float elevationHomeDeg = 0.0f;
  float azimuthBacklashOffset = 0.0f;
  float elevationBacklashOffset = 0.0f;
};

class Storage {
 public:
  void begin();
  const PersistentData &data() const { return data_; }
  void setData(const PersistentData &data);
  void commit();

 private:
  void load();

  PersistentData data_;
  bool dirty_ = false;
};

}  // namespace persist

