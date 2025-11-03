#pragma once

#include <Arduino.h>

#include "solar_calc.hpp"

namespace gps {

struct Fix {
  bool valid = false;
  double latitudeDeg = 0.0;
  double longitudeDeg = 0.0;
  double altitudeM = 0.0;
  solar::DateTime dateTime = {};
};

class Receiver {
 public:
  void begin(Stream &stream);
  void update();

  bool hasFix() const { return fix_.valid; }
  const Fix &fix() const { return fix_; }
  unsigned long lastUpdateMs() const { return lastUpdateMs_; }

 private:
  void processSentence();
  static double convertCoordinate(const char *token, const char hemisphere);

  Stream *stream_ = nullptr;
  Fix fix_;
  char buffer_[96];
  size_t bufferIndex_ = 0;
  unsigned long lastUpdateMs_ = 0;
};

}  // namespace gps

