#pragma once

#include <Arduino.h>
#include <cstring>
#include <array>
#include <vector>

#include "config.hpp"

namespace logger {

struct Entry {
    unsigned long timestamp_ms{0};
    char message[32]{};
};

inline std::array<Entry, config::MAX_LOG_ENTRIES> &buffer() {
    static std::array<Entry, config::MAX_LOG_ENTRIES> entries{};
    return entries;
}

inline size_t &head() {
    static size_t head_index = 0;
    return head_index;
}

inline void append(const char *msg) {
    Entry &entry = buffer()[head() % buffer().size()];
    entry.timestamp_ms = millis();
    strncpy(entry.message, msg, sizeof(entry.message) - 1);
    entry.message[sizeof(entry.message) - 1] = '\0';
    head() = (head() + 1) % buffer().size();
}

inline std::vector<const char *> tail(size_t count) {
    std::vector<const char *> out;
    size_t available = buffer().size();
    size_t start = (head() + buffer().size() - count) % buffer().size();
    for (size_t i = 0; i < count && i < available; ++i) {
        const Entry &entry = buffer()[(start + i) % buffer().size()];
        if (entry.message[0] == '\0') continue;
        out.push_back(entry.message);
    }
    return out;
}

} // namespace logger
