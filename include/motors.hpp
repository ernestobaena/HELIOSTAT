#pragma once

#include <Arduino.h>

#include "config.hpp"

namespace motors {

struct MotionCommand {
  float targetDeg;
  float maxSpeedDps;
  float maxAccelDps2;
};

class Axis {
 public:
  Axis(uint8_t pwmPin, uint8_t dirPin, uint8_t enablePin, float stepsPerDegree);

  void begin();
  void enable();
  void disable();

  void applyCommand(const MotionCommand &command);
  void stop();
  void update();

  float positionDeg() const { return positionDeg_; }
  bool isAtTarget(float toleranceDeg = 0.2f) const;
  bool isEnabled() const { return enabled_; }

 private:
  void driveMotor(float effort);

  uint8_t pwmPin_;
  uint8_t dirPin_;
  uint8_t enablePin_;
  float stepsPerDegree_;

  float positionDeg_ = 0.0f;
  float velocityDps_ = 0.0f;
  float targetDeg_ = 0.0f;
  float maxSpeedDps_ = 0.0f;
  float maxAccelDps2_ = 0.0f;
  bool enabled_ = false;
  unsigned long lastUpdateMs_ = 0;
};

}  // namespace motors

