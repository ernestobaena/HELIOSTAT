#pragma once

#include <Arduino.h>
#include <stdarg.h>

#include "config.hpp"

namespace logger {

enum class Level {
  kInfo,
  kWarn,
  kError,
};

struct Entry {
  unsigned long timestampMs = 0;
  Level level = Level::kInfo;
  char message[80];
};

class Logger {
 public:
  void begin(Stream &output);
  void log(Level level, const __FlashStringHelper *tag, const char *fmt, ...);
  void log(Level level, const char *tag, const char *fmt, ...);
  void dumpRecent(Stream &output) const;

 private:
  void addEntry(Level level, const char *tag, const char *fmt, va_list args);

  Entry entries_[config::LOGGER_MAX_ENTRIES];
  size_t head_ = 0;
  bool filled_ = false;
  Stream *output_ = nullptr;
};

}  // namespace logger

