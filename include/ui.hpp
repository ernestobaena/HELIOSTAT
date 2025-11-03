#pragma once

#include <Arduino.h>

#include "config.hpp"

namespace ui {

enum class Mode {
  kBoot,
  kIdle,
  kTracking,
  kParking,
  kFault,
};

enum class ButtonEvent {
  kNone,
  kShortPress,
  kLongPress,
};

struct Telemetry {
  float aimErrorDeg = 0.0f;
  float powerVoltage = 0.0f;
  bool gpsValid = false;
  bool safetyInhibit = false;
};

class UserInterface {
 public:
  void begin();
  void setMode(Mode mode);
  Mode mode() const { return mode_; }

  void update();
  void setTelemetry(const Telemetry &telemetry);
  ButtonEvent lastButtonEvent() const;
  void clearButtonEvent();

 private:
  void updateLed();

  Mode mode_ = Mode::kBoot;
  Telemetry telemetry_;
  ButtonEvent buttonEvent_ = ButtonEvent::kNone;
  unsigned long buttonPressStart_ = 0;
  bool buttonWasPressed_ = false;
  unsigned long lastLedToggle_ = 0;
  bool ledState_ = false;
};

}  // namespace ui

