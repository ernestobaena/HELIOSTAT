#include "power.hpp"

namespace power {

void Monitor::begin() {
  analogReadResolution(12);
  status_ = {};
  hasReading_ = false;
}

void Monitor::update() {
  const int raw = analogRead(config::PIN_POWER_SENSE);
  const float sensedVoltage = (static_cast<float>(raw) * 3.3f / 4095.0f) /
                              config::POWER_DIVIDER_RATIO;
  if (!hasReading_) {
    status_.voltage = sensedVoltage;
    hasReading_ = true;
  } else {
    status_.voltage = status_.voltage +
                      (sensedVoltage - status_.voltage) * config::POWER_FILTER_ALPHA;
  }
  status_.undervoltage = status_.voltage < config::POWER_MIN_OPERATING_VOLTAGE;
  status_.critical = status_.voltage < config::POWER_CRITICAL_VOLTAGE;
}

}  // namespace power

