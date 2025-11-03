#include "gps.hpp"

#include <stdlib.h>
#include <string.h>
#include <math.h>

namespace gps {

void Receiver::begin(Stream &stream) {
  stream_ = &stream;
  bufferIndex_ = 0;
  fix_ = {};
}

void Receiver::update() {
  if (!stream_) {
    return;
  }
  while (stream_->available()) {
    const char c = static_cast<char>(stream_->read());
    if (c == '\r') {
      continue;
    }
    if (c == '\n') {
      buffer_[bufferIndex_] = '\0';
      processSentence();
      bufferIndex_ = 0;
    } else if (bufferIndex_ < sizeof(buffer_) - 1) {
      buffer_[bufferIndex_++] = c;
    }
  }
}

void Receiver::processSentence() {
  if (bufferIndex_ == 0 || buffer_[0] != '$') {
    return;
  }

  char sentence[sizeof(buffer_)];
  strncpy(sentence, buffer_, sizeof(sentence));
  sentence[sizeof(sentence) - 1] = '\0';

  char *saveptr = nullptr;
  char *token = strtok_r(sentence, ",", &saveptr);
  if (!token) return;

  if (strcmp(token + 3, "RMC") == 0) {
    char *timeStr = strtok_r(nullptr, ",", &saveptr);
    char *status = strtok_r(nullptr, ",", &saveptr);
    char *latStr = strtok_r(nullptr, ",", &saveptr);
    char *latHem = strtok_r(nullptr, ",", &saveptr);
    char *lonStr = strtok_r(nullptr, ",", &saveptr);
    char *lonHem = strtok_r(nullptr, ",", &saveptr);
    strtok_r(nullptr, ",", &saveptr);  // speed
    strtok_r(nullptr, ",", &saveptr);  // course
    char *dateStr = strtok_r(nullptr, ",", &saveptr);

    if (!timeStr || !status || !dateStr || status[0] != 'A') {
      fix_.valid = false;
      return;
    }

    fix_.latitudeDeg = convertCoordinate(latStr, latHem ? latHem[0] : 'N');
    fix_.longitudeDeg = convertCoordinate(lonStr, lonHem ? lonHem[0] : 'E');
    fix_.valid = true;

    if (timeStr && strlen(timeStr) >= 6) {
      fix_.dateTime.hour = (timeStr[0] - '0') * 10 + (timeStr[1] - '0');
      fix_.dateTime.minute = (timeStr[2] - '0') * 10 + (timeStr[3] - '0');
      fix_.dateTime.second = (timeStr[4] - '0') * 10 + (timeStr[5] - '0');
    }
    if (dateStr && strlen(dateStr) == 6) {
      fix_.dateTime.day = (dateStr[0] - '0') * 10 + (dateStr[1] - '0');
      fix_.dateTime.month = (dateStr[2] - '0') * 10 + (dateStr[3] - '0');
      const int year = (dateStr[4] - '0') * 10 + (dateStr[5] - '0');
      fix_.dateTime.year = (year >= 80 ? 1900 : 2000) + year;
    }
    lastUpdateMs_ = millis();
  } else if (strcmp(token + 3, "GGA") == 0) {
    strtok_r(nullptr, ",", &saveptr);  // time
    char *latStr = strtok_r(nullptr, ",", &saveptr);
    char *latHem = strtok_r(nullptr, ",", &saveptr);
    char *lonStr = strtok_r(nullptr, ",", &saveptr);
    char *lonHem = strtok_r(nullptr, ",", &saveptr);
    char *fixQuality = strtok_r(nullptr, ",", &saveptr);
    strtok_r(nullptr, ",", &saveptr);  // satellites
    strtok_r(nullptr, ",", &saveptr);  // hdop
    char *altStr = strtok_r(nullptr, ",", &saveptr);

    if (!fixQuality || fixQuality[0] == '0') {
      fix_.valid = false;
      return;
    }

    fix_.latitudeDeg = convertCoordinate(latStr, latHem ? latHem[0] : 'N');
    fix_.longitudeDeg = convertCoordinate(lonStr, lonHem ? lonHem[0] : 'E');
    if (altStr && altStr[0] != '\0') {
      fix_.altitudeM = atof(altStr);
    }
    fix_.valid = true;
    lastUpdateMs_ = millis();
  }
}

double Receiver::convertCoordinate(const char *token, const char hemisphere) {
  if (!token || token[0] == '\0') {
    return 0.0;
  }
  const double raw = atof(token);
  const double degrees = floor(raw / 100.0);
  const double minutes = raw - degrees * 100.0;
  double result = degrees + minutes / 60.0;
  if (hemisphere == 'S' || hemisphere == 'W') {
    result = -result;
  }
  return result;
}

}  // namespace gps

