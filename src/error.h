#ifndef TARO_ERROR
#define TARO_ERROR

#include <stdarg.h>
#include <stdio.h>

enum LogLevel
{
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR,
};

static inline void taro_log(enum LogLevel level, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    switch (level)
    {
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
    va_end(args);
}

static inline void log_info(const char *fmt, ...)
{
    taro_log(LOG_INFO, fmt);
}

static inline void log_warn(const char *fmt, ...)
{
    taro_log(LOG_WARNING, fmt);
}

static inline void log_error(const char *fmt, ...)
{
    taro_log(LOG_ERROR, fmt);
}

#endif