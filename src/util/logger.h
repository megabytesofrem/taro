#ifndef TARO_UTIL_LOGGER_H
#define TARO_UTIL_LOGGER_H

#include <stdarg.h>
#include <stdio.h>

enum LogLevel {
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR,
};

static inline void taro_log(enum LogLevel level, const char *fmt, va_list args) {
    switch (level) {
    case LOG_INFO:
        printf("[INFO] ");
        break;
    case LOG_WARNING:
        printf("[WARN] ");
        break;
    case LOG_ERROR:
        printf("[ERROR] ");
        break;
    }

    vprintf(fmt, args);
}

static inline void log_info(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    taro_log(LOG_INFO, fmt, args);
    va_end(args);
}

static inline void log_warn(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    taro_log(LOG_WARNING, fmt, args);
    va_end(args);
}

static inline void log_error(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    taro_log(LOG_ERROR, fmt, args);
    va_end(args);
}

#endif
