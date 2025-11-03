#pragma once

#include <Arduino.h>

#include "config.hpp"

namespace safety {

struct Status {
  bool emergencyStop = false;
  bool azimuthLimit = false;
  bool elevationLimit = false;
  float windSpeedMps = 0.0f;
  float enclosureTempC = 0.0f;
  bool inhibited = false;
};

class Manager {
 public:
  void begin();
  void update();

  const Status &status() const { return status_; }
  bool allowMotion() const { return !status_.inhibited; }

 private:
  Status status_;
};

}  // namespace safety

