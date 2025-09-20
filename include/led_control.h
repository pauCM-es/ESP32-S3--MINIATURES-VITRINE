#ifndef LED_CONTROL_H
#define LED_CONTROL_H

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "config.h"

class LedControl {
private:
    Adafruit_NeoPixel* strip;

public:
    // Constructor
    LedControl();
    
    // Initialize LED strip
    void begin();
    
    // Set brightness (0-255)
    void setBrightness(uint8_t brightness);
    
    // Clear all LEDs (using strip->clear())
    void clear();
    
    // Light a specific position with a color
    void lightPosition(int position, uint32_t color);
    
    // Clear all LEDs (turn them off by setting to 0)
    void clearAll();
    
    // Color creation and getters
    uint32_t getColor(uint8_t r, uint8_t g, uint8_t b);
    uint32_t getRed();
    uint32_t getGreen();
    uint32_t getBlue();
    uint32_t getYellow();
    uint32_t getOff();
};

#endif // LED_CONTROL_H
