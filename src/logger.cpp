#include "logger.hpp"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#if defined(__AVR__)
#include <avr/pgmspace.h>
#endif

namespace logger {

void Logger::begin(Stream &output) { output_ = &output; }

void Logger::log(Level level, const __FlashStringHelper *tag, const char *fmt, ...) {
  char tagBuffer[16];
#if defined(__AVR__)
  strncpy_P(tagBuffer, reinterpret_cast<PGM_P>(tag), sizeof(tagBuffer) - 1);
#else
  strncpy(tagBuffer, reinterpret_cast<const char *>(tag), sizeof(tagBuffer) - 1);
#endif
  tagBuffer[sizeof(tagBuffer) - 1] = '\0';

  va_list args;
  va_start(args, fmt);
  addEntry(level, tagBuffer, fmt, args);
  va_end(args);
}

void Logger::log(Level level, const char *tag, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  addEntry(level, tag, fmt, args);
  va_end(args);
}

void Logger::addEntry(Level level, const char *tag, const char *fmt, va_list args) {
  Entry &entry = entries_[head_];
  entry.timestampMs = millis();
  entry.level = level;

  char formatted[sizeof(entry.message)];
  va_list argsCopy;
  va_copy(argsCopy, args);
  vsnprintf(formatted, sizeof(formatted), fmt, argsCopy);
  va_end(argsCopy);
  snprintf(entry.message, sizeof(entry.message), "%s: %s", tag, formatted);

  if (output_) {
    output_->print('[');
    output_->print(entry.timestampMs);
    output_->print(F("] "));
    switch (level) {
      case Level::kInfo:
        output_->print(F("INFO "));
        break;
      case Level::kWarn:
        output_->print(F("WARN "));
        break;
      case Level::kError:
        output_->print(F("ERROR "));
        break;
    }
    output_->println(entry.message);
  }

  head_ = (head_ + 1) % config::LOGGER_MAX_ENTRIES;
  if (head_ == 0) {
    filled_ = true;
  }
}

void Logger::dumpRecent(Stream &output) const {
  const size_t count = filled_ ? config::LOGGER_MAX_ENTRIES : head_;
  for (size_t i = 0; i < count; ++i) {
    size_t index = (head_ + config::LOGGER_MAX_ENTRIES - count + i) %
                   config::LOGGER_MAX_ENTRIES;
    const Entry &entry = entries_[index];
    output.print('[');
    output.print(entry.timestampMs);
    output.print(F("] "));
    switch (entry.level) {
      case Level::kInfo:
        output.print(F("INFO "));
        break;
      case Level::kWarn:
        output.print(F("WARN "));
        break;
      case Level::kError:
        output.print(F("ERROR "));
        break;
    }
    output.println(entry.message);
  }
}

}  // namespace logger

