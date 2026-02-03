#ifndef DEVICE_SETTINGS_H
#define DEVICE_SETTINGS_H

#include <Arduino.h>

struct DeviceSettings {
    // 0 == disabled
    uint16_t sleepTimeoutMinutes = 5;

    // 0..100
    uint8_t backlightBrightnessPercent = 100;

    // Ambient settings
    uint8_t ambientAllLightsBrightnessPercent = 25;

    // Random ambient pattern settings
    uint8_t ambientRandomMaxBrightnessPercent = 50;
    uint8_t ambientRandomDensity = 6;

    // Animation speed (lower == faster)
    uint16_t ambientRandomFrameMs = 40;
    uint8_t ambientRandomStep = 6;
};

#endif
