#include "color_utils.h"
#include <Adafruit_NeoPixel.h>

namespace color_utils {

// Blend RGB and White channels
uint32_t blendColorWithWhite(Adafruit_NeoPixel* strip, uint8_t r, uint8_t g, uint8_t b, uint8_t w) {
    return strip->Color(r, g, b, w);
}

// Calculate white intensity dynamically
uint8_t calculateWhiteIntensity(uint8_t r, uint8_t g, uint8_t b) {
    return (r + g + b) / 3;
}

// Get color with auto white adjustment
uint32_t getColorWithAutoWhite(Adafruit_NeoPixel* strip, uint8_t r, uint8_t g, uint8_t b) {
    uint8_t w = calculateWhiteIntensity(r, g, b);
    return strip->Color(r, g, b, w);
}

// Adjust color temperature
uint32_t adjustColorTemperature(Adafruit_NeoPixel* strip, uint8_t r, uint8_t g, uint8_t b, int temperature) {
    uint8_t w = constrain(temperature, 0, 255);
    return strip->Color(r, g, b, w);
}

// Fade between two colors
uint32_t fadeToColor(Adafruit_NeoPixel* strip, uint32_t color1, uint32_t color2, float ratio) {
    uint8_t r1 = (color1 >> 16) & 0xFF;
    uint8_t g1 = (color1 >> 8) & 0xFF;
    uint8_t b1 = color1 & 0xFF;
    uint8_t w1 = (color1 >> 24) & 0xFF;

    uint8_t r2 = (color2 >> 16) & 0xFF;
    uint8_t g2 = (color2 >> 8) & 0xFF;
    uint8_t b2 = color2 & 0xFF;
    uint8_t w2 = (color2 >> 24) & 0xFF;

    uint8_t r = r1 + (r2 - r1) * ratio;
    uint8_t g = g1 + (g2 - g1) * ratio;
    uint8_t b = b1 + (b2 - b1) * ratio;
    uint8_t w = w1 + (w2 - w1) * ratio;

    return strip->Color(r, g, b, w);
}

// Set warm white
void setWarmWhite(Adafruit_NeoPixel* strip, uint8_t brightness) {
    for (int i = 0; i < strip->numPixels(); i++) {
        strip->setPixelColor(i, strip->Color(255, 223, 191, brightness));
    }
    strip->show();
}

// Set cool white
void setCoolWhite(Adafruit_NeoPixel* strip, uint8_t brightness) {
    for (int i = 0; i < strip->numPixels(); i++) {
        strip->setPixelColor(i, strip->Color(191, 223, 255, brightness));
    }
    strip->show();
}

// Breathing white effect
void breathingWhite(Adafruit_NeoPixel* strip, uint8_t minBrightness, uint8_t maxBrightness, uint16_t delayMs) {
    for (int brightness = minBrightness; brightness <= maxBrightness; brightness++) {
        for (int i = 0; i < strip->numPixels(); i++) {
            strip->setPixelColor(i, strip->Color(0, 0, 0, brightness));
        }
        strip->show();
        delay(delayMs);
    }
    for (int brightness = maxBrightness; brightness >= minBrightness; brightness--) {
        for (int i = 0; i < strip->numPixels(); i++) {
            strip->setPixelColor(i, strip->Color(0, 0, 0, brightness));
        }
        strip->show();
        delay(delayMs);
    }
}

// Get white color with specified brightness
uint32_t getWhiteColor(Adafruit_NeoPixel* strip, uint8_t brightness) {
    return strip->Color(0, 0, 0, brightness); // White channel only
}

} // namespace color_utils