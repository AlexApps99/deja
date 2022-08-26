#pragma once

#include "pico/stdio_usb.h"
#include "stdio.h"

#define MAX_FMT_LEN 512

void logstr(unsigned char level, const char* buffer);
#define logmsg(level, ...)                              \
    {                                                   \
        char buffer[MAX_FMT_LEN];                       \
        snprintf(&buffer[0], MAX_FMT_LEN, __VA_ARGS__); \
        logstr(level, buffer);                          \
    }
#define LogLevel_ERROR 0
#define LogLevel_WARN 1
#define LogLevel_INFO 2
#define LogLevel_DEBUG 3
#define Lerror(msg, ...) logmsg(LogLevel_ERROR, msg, ##__VA_ARGS__)
#define Lwarn(msg, ...) logmsg(LogLevel_WARN, msg, ##__VA_ARGS__)
#define Linfo(msg, ...) logmsg(LogLevel_INFO, msg, ##__VA_ARGS__)
#define Ldebug(msg, ...) logmsg(LogLevel_DEBUG, msg, ##__VA_ARGS__)
