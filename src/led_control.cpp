#include "led_control.h"

// Constructor
LedControl::LedControl() {
    strip = new Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);
}

// Initialize LED strip
void LedControl::begin() {
    strip->begin();
    strip->clear();
    strip->show();
    strip->setBrightness(12); // Default to 12/255 brightness (about 5%)
}

// Set brightness (0-255)
void LedControl::setBrightness(uint8_t brightness) {
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
    
    clearAll();
    
    // Deshabilitar interrupciones durante la actualización del LED
    noInterrupts();
    strip->setPixelColor(position, color);
    strip->show();
    interrupts();
}

// Add clearAll method implementation to turn off all LEDs
void LedControl::clearAll() {
    // Deshabilitar interrupciones durante la actualización de LEDs
    noInterrupts();
    for (int i = 0; i < NUM_LEDS; i++) {
        strip->setPixelColor(i, 0);
    }
    strip->show();
    interrupts();
}

// Run a simple animation sequence
void LedControl::runAnimation(int cycles) {
    for (int i = 0; i < cycles; i++) {
        // Cycle through positions
        for (int pos = 0; pos < NUM_LEDS; pos++) {
            clearAll();
            strip->setPixelColor(pos, strip->Color(255, 255, 255));
            strip->show();
            delay(ANIMATION_SPEED);
        }
    }
    clear();
}

// Color helper methods
uint32_t LedControl::getColor(uint8_t r, uint8_t g, uint8_t b) {
    return strip->Color(r, g, b);
}

uint32_t LedControl::getRed() {
    return strip->Color(255, 0, 0);
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

uint32_t LedControl::getOff() {
    return strip->Color(0, 0, 0);
}
