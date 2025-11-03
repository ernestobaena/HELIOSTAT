#include "motors.hpp"

#include <math.h>

namespace motors {

Axis::Axis(uint8_t pwmPin, uint8_t dirPin, uint8_t enablePin, float stepsPerDegree)
    : pwmPin_(pwmPin),
      dirPin_(dirPin),
      enablePin_(enablePin),
      stepsPerDegree_(stepsPerDegree) {}

void Axis::begin() {
  pinMode(pwmPin_, OUTPUT);
  pinMode(dirPin_, OUTPUT);
  pinMode(enablePin_, OUTPUT);
  disable();
  lastUpdateMs_ = millis();
}

void Axis::enable() {
  digitalWrite(enablePin_, LOW);
  enabled_ = true;
}

void Axis::disable() {
  digitalWrite(enablePin_, HIGH);
  enabled_ = false;
  driveMotor(0.0f);
}

void Axis::applyCommand(const MotionCommand &command) {
  targetDeg_ = command.targetDeg;
  maxSpeedDps_ = max(0.1f, command.maxSpeedDps);
  maxAccelDps2_ = max(0.1f, command.maxAccelDps2);
}

void Axis::stop() {
  targetDeg_ = positionDeg_;
  velocityDps_ = 0.0f;
  driveMotor(0.0f);
}

bool Axis::isAtTarget(float toleranceDeg) const {
  return fabs(targetDeg_ - positionDeg_) <= toleranceDeg;
}

void Axis::update() {
  const unsigned long now = millis();
  const float dt = (now == lastUpdateMs_) ? 0.0f : (now - lastUpdateMs_) / 1000.0f;
  lastUpdateMs_ = now;

  if (!enabled_) {
    velocityDps_ = 0.0f;
    return;
  }

  const float error = targetDeg_ - positionDeg_;
  const float direction = (error >= 0.0f) ? 1.0f : -1.0f;

  const float desiredVelocity = direction * maxSpeedDps_;
  if (velocityDps_ < desiredVelocity) {
    velocityDps_ = min(velocityDps_ + maxAccelDps2_ * dt, desiredVelocity);
  } else if (velocityDps_ > desiredVelocity) {
    velocityDps_ = max(velocityDps_ - maxAccelDps2_ * dt, desiredVelocity);
  }

  const float stoppingDistance = (velocityDps_ * velocityDps_) / (2.0f * maxAccelDps2_ + 1e-6f);
  if (fabs(error) <= stoppingDistance) {
    velocityDps_ = max(0.0f, velocityDps_ - maxAccelDps2_ * dt);
  }

  positionDeg_ += velocityDps_ * dt;

  if ((direction > 0 && positionDeg_ > targetDeg_) ||
      (direction < 0 && positionDeg_ < targetDeg_)) {
    positionDeg_ = targetDeg_;
    velocityDps_ = 0.0f;
  }

  if (fabs(error) < 0.05f && velocityDps_ < 0.05f) {
    positionDeg_ = targetDeg_;
    velocityDps_ = 0.0f;
  }

  const float normalizedEffort = (maxSpeedDps_ > 0.0f)
                                     ? constrain(velocityDps_ / maxSpeedDps_, -1.0f, 1.0f)
                                     : 0.0f;
  driveMotor(normalizedEffort);
}

void Axis::driveMotor(float effort) {
  effort = constrain(effort, -1.0f, 1.0f);
  if (effort == 0.0f) {
    analogWrite(pwmPin_, 0);
    return;
  }
  const bool forward = effort > 0.0f;
  digitalWrite(dirPin_, forward ? HIGH : LOW);
  analogWrite(pwmPin_, static_cast<int>(fabs(effort) * 255));
}

}  // namespace motors

