#ifndef COLOR_UTILS_H
#define COLOR_UTILS_H

#include <Adafruit_NeoPixel.h>
#include <Arduino.h>

namespace color_utils {

// Blend RGB and White channels
uint32_t blendColorWithWhite(Adafruit_NeoPixel* strip, uint8_t r, uint8_t g, uint8_t b, uint8_t w);

// Calculate white intensity dynamically
uint8_t calculateWhiteIntensity(uint8_t r, uint8_t g, uint8_t b);

// Get color with auto white adjustment
uint32_t getColorWithAutoWhite(Adafruit_NeoPixel* strip, uint8_t r, uint8_t g, uint8_t b);

// Adjust color temperature
uint32_t adjustColorTemperature(Adafruit_NeoPixel* strip, uint8_t r, uint8_t g, uint8_t b, int temperature);

// Fade between two colors
uint32_t fadeToColor(Adafruit_NeoPixel* strip, uint32_t color1, uint32_t color2, float ratio);

// Set warm white
void setWarmWhite(Adafruit_NeoPixel* strip, uint8_t brightness);

// Set cool white
void setCoolWhite(Adafruit_NeoPixel* strip, uint8_t brightness);

// Breathing white effect
void breathingWhite(Adafruit_NeoPixel* strip, uint8_t minBrightness, uint8_t maxBrightness, uint16_t delayMs);

// Get white color with specified brightness
uint32_t getWhiteColor(Adafruit_NeoPixel* strip, uint8_t brightness);

} // namespace color_utils

#endif