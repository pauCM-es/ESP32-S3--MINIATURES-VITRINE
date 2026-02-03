#ifndef DEVICE_SETTINGS_H
#define DEVICE_SETTINGS_H

#include <Arduino.h>

// Conservative sizes (ESP32 WiFi): SSID <= 32, WPA2 pass <= 63
static constexpr size_t WIFI_SSID_MAX_LEN = 32;
static constexpr size_t WIFI_PASS_MAX_LEN = 64;

struct DeviceSettings {
    // 0 == disabled
    uint16_t sleepTimeoutMinutes = 5;

    // 0..100
    uint8_t backlightBrightnessPercent = 100;

    // 0..100: global LED strip brightness scaling (Adafruit_NeoPixel brightness)
    uint8_t ledBrightnessPercent = 40;

    // 0..100: used for standby/rest lighting brightness
    uint8_t standbyBrightnessPercent = 30;

    // Last UI state
    uint8_t lastMiniatureIndex = 0;
    int8_t lastMainModeIndex = -1;

    // Ambient settings
    uint8_t ambientAllLightsBrightnessPercent = 25;

    // Random ambient pattern settings
    uint8_t ambientRandomMaxBrightnessPercent = 50;
    uint8_t ambientRandomDensity = 6;

    // Animation speed (lower == faster)
    uint16_t ambientRandomFrameMs = 40;
    uint8_t ambientRandomStep = 6;

    // WiFi settings
    bool wifiStaEnabled = true;
    char wifiStaSsid[WIFI_SSID_MAX_LEN + 1] = {0};
    char wifiStaPass[WIFI_PASS_MAX_LEN + 1] = {0};

    char wifiApSsid[WIFI_SSID_MAX_LEN + 1] = "Vitrine-ESP32S3";
    char wifiApPass[WIFI_PASS_MAX_LEN + 1] = "vitrine1234";
};

#endif
