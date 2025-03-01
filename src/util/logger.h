#ifndef UTIL_LOGGER_H
#define UTIL_LOGGER_H

#include <stdarg.h>
#include <stdio.h>

#if defined(__linux__) || defined(__unix__) || defined(__APPLE__) || defined(__MACH__)
/* Escape codes are already supported well on *NIX platforms */
#define C_RESET "\033[0m"
#define C_RED "\033[31m"
#define C_YELLOW "\033[33m"

#elif defined(_WIN32)
#include <windows.h>

/* TODO: Add Windows support
 * https://solarianprogrammer.com/2019/04/08/c-programming-ansi-escape-codes-windows-macos-linux-terminals/
 */
#else
#endif

#define COLOR(text, color) color text C_RESET

enum LogLevel {
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR,
};

static inline void taro_log(enum LogLevel level, const char *fmt, ...) {
    switch (level) {
    case LOG_INFO:
        printf(COLOR("[INFO] ", C_YELLOW));
        break;
    case LOG_WARN:
        printf(COLOR("[WARN] ", C_YELLOW));
        break;
    case LOG_ERROR:
        printf(COLOR("[ERROR] ", C_RED));
        break;
    }

    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}

#define log_info(fmt, ...) taro_log(LOG_INFO, fmt, ##__VA_ARGS__)
#define log(fmt, ...) log_info(fmt, ##__VA_ARGS__)
#define log_warn(fmt, ...) taro_log(LOG_WARN, fmt, ##__VA_ARGS__)
#define log_error(fmt, ...) taro_log(LOG_ERROR, fmt, ##__VA_ARGS__)

#endif
