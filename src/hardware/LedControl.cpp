#include "LedControl.h"
#include "ColorUtils.h" // Include the new color_utils header

namespace {
uint8_t addSaturating(uint8_t base, uint8_t extra) {
    uint16_t sum = static_cast<uint16_t>(base) + static_cast<uint16_t>(extra);
    return (sum > 255U) ? 255U : static_cast<uint8_t>(sum);
}
}

// Constructor
LedControl::LedControl() {
    // WS2812B is RGB with GRB byte order.
    strip = new Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);
}

// Initialize LED strip
void LedControl::begin() {
    strip->begin();
    strip->clear();
    strip->setBrightness(12); // ~5% default brightness to limit inrush/current.
    strip->show();
}

// Set brightness (0-255)
void LedControl::setBrightness(uint8_t brightnessPercentage) {
    uint8_t brightness = map(brightnessPercentage, 0, 100, 0, 255);
    strip->setBrightness(brightness);
}

// Clear all LEDs
void LedControl::clear() {
    strip->clear();
    strip->show();
}

// Light up a specific position
void LedControl::lightPosition(int position, uint32_t color) {
    if (position < 0 || position >= NUM_LEDS) {
        return;
    }
    
    strip->setPixelColor(position, color);
    strip->show();
}

void LedControl::setPixel(int position, uint32_t color) {
    if (position < 0 || position >= NUM_LEDS) {
        return;
    }
    strip->setPixelColor(position, color);
}

void LedControl::setPixelRGBW(int position, uint8_t r, uint8_t g, uint8_t b, uint8_t w) {
    if (position < 0 || position >= NUM_LEDS) {
        return;
    }
    // WS2812B has no dedicated white channel; mix white into RGB.
    strip->setPixelColor(
        position,
        addSaturating(r, w),
        addSaturating(g, w),
        addSaturating(b, w)
    );
}

void LedControl::setPixelWhite(int position, uint8_t w) {
    setPixelRGBW(position, 0, 0, 0, w);
}

void LedControl::show() {
    strip->show();
}

void LedControl::fill(uint32_t color) {
    strip->fill(color);
    strip->show();
}

// Add clearAll method implementation to turn off all LEDs
void LedControl::clearAll() {
    for (int i = 0; i < NUM_LEDS; i++) {
        strip->setPixelColor(i, 0);
    }
    strip->show();
}

void LedControl::setWhite(uint8_t brightnessPercentage) {
    uint8_t brightness = map(brightnessPercentage, 0, 100, 0, 255);
    strip->fill(strip->Color(brightness, brightness, brightness));
    strip->show();
}


// Color helper methods
uint32_t LedControl::getColor(uint8_t r, uint8_t g, uint8_t b, uint8_t w) {
    return strip->Color(
        addSaturating(r, w),
        addSaturating(g, w),
        addSaturating(b, w)
    );
}

uint32_t LedControl::getWhite(uint8_t brightnessPercentage) {
    uint8_t brightness = map(brightnessPercentage, 0, 100, 0, 255);
    return strip->Color(brightness, brightness, brightness);
}

uint32_t LedControl::getRed() {
    return strip->Color(255,0, 0);
}

uint32_t LedControl::getGreen() {
    return strip->Color(0, 255, 0);
}

uint32_t LedControl::getBlue() {
    return strip->Color(0, 0, 255);
}

uint32_t LedControl::getYellow() {
    return strip->Color(255, 255, 0);
}

uint32_t LedControl::getWhiteRGB() {
    return strip->Color(255, 255, 255);
}

uint32_t LedControl::getOff() {
    return  strip->Color(0, 0, 0);
}


