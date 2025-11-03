#pragma once

#include <Arduino.h>

#include "config.hpp"

namespace power {

struct Status {
  float voltage = 0.0f;
  bool undervoltage = false;
  bool critical = false;
};

class Monitor {
 public:
  void begin();
  void update();
  Status status() const { return status_; }

 private:
  Status status_;
  bool hasReading_ = false;
};

}  // namespace power

