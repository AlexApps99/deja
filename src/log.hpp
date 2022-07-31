#pragma once

enum class LogLevel : unsigned char {
    ERROR = 0,
    WARN = 1,
    INFO = 2,
    DEBUG = 3,
};

class Log {
   public:
    // Logs a message (can be called from multiple threads)
    virtual void log(const LogLevel level, const char* msg,
                     const char* const pos);

    inline static Log& instance() { return logger; }

    // Set the global logger to this
    inline void set_global() { logger = *this; }

    // Names for log levels
    constexpr static const char* const level(const LogLevel level);

    // Stdio logger
    static void set_stdio();

   private:
    static Log& logger;
};

extern "C" {
#include "log.h"
}
