#include "ui.hpp"

#include <Arduino.h>

namespace ui {

void UserInterface::begin() {
  pinMode(config::PIN_UI_BUTTON, INPUT_PULLUP);
  pinMode(config::PIN_UI_STATUS_LED, OUTPUT);
  digitalWrite(config::PIN_UI_STATUS_LED, LOW);
  mode_ = Mode::kBoot;
  telemetry_ = {};
}

void UserInterface::setMode(Mode mode) {
  if (mode_ != mode) {
    mode_ = mode;
    lastLedToggle_ = 0;
  }
}

void UserInterface::setTelemetry(const Telemetry &telemetry) {
  telemetry_ = telemetry;
}

void UserInterface::update() {
  const bool buttonPressed = digitalRead(config::PIN_UI_BUTTON) == LOW;
  const unsigned long now = millis();
  if (buttonPressed && !buttonWasPressed_) {
    buttonPressStart_ = now;
  } else if (!buttonPressed && buttonWasPressed_) {
    const unsigned long duration = now - buttonPressStart_;
    if (duration > 50 && duration < 1000) {
      buttonEvent_ = ButtonEvent::kShortPress;
    } else if (duration >= 1000) {
      buttonEvent_ = ButtonEvent::kLongPress;
    }
  }
  buttonWasPressed_ = buttonPressed;

  updateLed();

  static unsigned long lastReport = 0;
  if (now - lastReport > 1000) {
    lastReport = now;
    Serial.print(F("UI mode="));
    Serial.print(static_cast<int>(mode_));
    Serial.print(F(" aimErr="));
    Serial.print(telemetry_.aimErrorDeg, 2);
    Serial.print(F(" power="));
    Serial.print(telemetry_.powerVoltage, 2);
    Serial.print(F(" gps="));
    Serial.print(telemetry_.gpsValid ? F("OK") : F("----"));
    Serial.print(F(" safe="));
    Serial.println(telemetry_.safetyInhibit ? F("INHIBIT") : F("OK"));
  }
}

ui::ButtonEvent UserInterface::lastButtonEvent() const {
  return buttonEvent_;
}

void UserInterface::clearButtonEvent() {
  buttonEvent_ = ButtonEvent::kNone;
}

void UserInterface::updateLed() {
  unsigned long now = millis();
  unsigned long interval = 1000;
  switch (mode_) {
    case Mode::kBoot:
      interval = 150;
      break;
    case Mode::kIdle:
      interval = 1000;
      break;
    case Mode::kTracking:
      interval = 250;
      break;
    case Mode::kParking:
      interval = 500;
      break;
    case Mode::kFault:
      interval = 100;
      break;
  }

  if (now - lastLedToggle_ >= interval) {
    lastLedToggle_ = now;
    ledState_ = !ledState_;
    digitalWrite(config::PIN_UI_STATUS_LED, ledState_ ? HIGH : LOW);
  }
}

}  // namespace ui

