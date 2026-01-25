#include "log.h"

HardwareSerial *Log::s_serial = nullptr;

void Log::begin(HardwareSerial &serial, uint32_t baud) {
    s_serial = &serial;
    s_serial->begin(baud);

    // Small delay helps ensure the USB/serial monitor is ready.
    delay(50);
}

void Log::error(const char *tag, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    write(LogLevel::Error, tag, fmt, args);
    va_end(args);
}

void Log::warn(const char *tag, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    write(LogLevel::Warn, tag, fmt, args);
    va_end(args);
}

void Log::info(const char *tag, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    write(LogLevel::Info, tag, fmt, args);
    va_end(args);
}

void Log::debug(const char *tag, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    write(LogLevel::Debug, tag, fmt, args);
    va_end(args);
}

void Log::write(LogLevel level, const char *tag, const char *fmt, va_list args) {
    if (!s_serial) {
        return;
    }

    const uint8_t compiledLevel = static_cast<uint8_t>(level);
    if (compiledLevel > static_cast<uint8_t>(LOG_LEVEL)) {
        return;
    }

    char msg[256];
    vsnprintf(msg, sizeof(msg), fmt, args);

    char levelChar = '?';
    switch (level) {
        case LogLevel::Error: levelChar = 'E'; break;
        case LogLevel::Warn:  levelChar = 'W'; break;
        case LogLevel::Info:  levelChar = 'I'; break;
        case LogLevel::Debug: levelChar = 'D'; break;
    }

    const uint32_t ms = millis();

    s_serial->print('[');
    s_serial->print(ms);
    s_serial->print("][");
    s_serial->print(levelChar);
    s_serial->print("]");

    if (tag && tag[0] != '\0') {
        s_serial->print('[');
        s_serial->print(tag);
        s_serial->print(']');
    }

    s_serial->print(' ');
    s_serial->println(msg);
}
