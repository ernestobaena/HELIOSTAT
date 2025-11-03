#include "safety.hpp"

namespace safety {

void Manager::begin() {
  pinMode(config::PIN_SAFETY_ESTOP, INPUT_PULLUP);
  pinMode(config::PIN_SAFETY_AZ_LIMIT, INPUT_PULLUP);
  pinMode(config::PIN_SAFETY_EL_LIMIT, INPUT_PULLUP);
  status_ = {};
}

void Manager::update() {
  status_.emergencyStop = digitalRead(config::PIN_SAFETY_ESTOP) == LOW;
  status_.azimuthLimit = digitalRead(config::PIN_SAFETY_AZ_LIMIT) == LOW;
  status_.elevationLimit = digitalRead(config::PIN_SAFETY_EL_LIMIT) == LOW;

  const int rawWind = analogRead(config::PIN_WIND_SPEED);
  const float windVoltage = rawWind * 3.3f / 4095.0f;
  status_.windSpeedMps = constrain(windVoltage / 3.3f * 25.0f, 0.0f, 40.0f);

  const int rawTemp = analogRead(config::PIN_TEMPERATURE);
  const float tempVoltage = rawTemp * 3.3f / 4095.0f;
  status_.enclosureTempC = constrain(tempVoltage / 3.3f * 100.0f, -40.0f, 125.0f);

  status_.inhibited = status_.emergencyStop || status_.azimuthLimit ||
                      status_.elevationLimit ||
                      status_.windSpeedMps > config::SAFETY_WIND_SPEED_LIMIT ||
                      status_.enclosureTempC > config::SAFETY_TEMPERATURE_LIMIT;
}

}  // namespace safety

