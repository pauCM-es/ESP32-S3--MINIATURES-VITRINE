#pragma once

#include <Arduino.h>
#include <cstdarg>

// Compile-time log level (0=ERROR, 1=WARN, 2=INFO, 3=DEBUG)
#ifndef LOG_LEVEL
#define LOG_LEVEL 2
#endif

enum class LogLevel : uint8_t {
    Error = 0,
    Warn = 1,
    Info = 2,
    Debug = 3,
};

class Log {
public:
    static void begin(HardwareSerial &serial, uint32_t baud = 115200);

    static void error(const char *tag, const char *fmt, ...);
    static void warn(const char *tag, const char *fmt, ...);
    static void info(const char *tag, const char *fmt, ...);
    static void debug(const char *tag, const char *fmt, ...);

private:
    static void write(LogLevel level, const char *tag, const char *fmt, va_list args);

    static HardwareSerial *s_serial;
};

#if LOG_LEVEL >= 0
#define LOGE(TAG, FMT, ...) Log::error((TAG), (FMT), ##__VA_ARGS__)
#else
#define LOGE(TAG, FMT, ...) do { } while (0)
#endif

#if LOG_LEVEL >= 1
#define LOGW(TAG, FMT, ...) Log::warn((TAG), (FMT), ##__VA_ARGS__)
#else
#define LOGW(TAG, FMT, ...) do { } while (0)
#endif

#if LOG_LEVEL >= 2
#define LOGI(TAG, FMT, ...) Log::info((TAG), (FMT), ##__VA_ARGS__)
#else
#define LOGI(TAG, FMT, ...) do { } while (0)
#endif

#if LOG_LEVEL >= 3
#define LOGD(TAG, FMT, ...) Log::debug((TAG), (FMT), ##__VA_ARGS__)
#else
#define LOGD(TAG, FMT, ...) do { } while (0)
#endif
