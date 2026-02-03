#include "LedMovementControl.h"
#include "LedControl.h"

// Constructor
LedMovementControl::LedMovementControl(LedControl& ledControl) : ledControl(ledControl) {}

// Set all LEDs to standby mode (50% brightness)
void LedMovementControl::setStandbyMode(int brightness) {
    pattern = Pattern::Standby;
    ledControl.setWhite(brightness);
}

// Highlight a specific miniature (focus mode)
void LedMovementControl::setFocusMode(int position, bool lightUpRest) {
    pattern = Pattern::Focus;
    if (lightUpRest) {
        setStandbyMode(30);
        isStandbyLight = true;
    } else {
        ledControl.clearAll();
        isStandbyLight = false;
    }
    
    ledControl.lightPosition(position, ledControl.getWhite(100)); // 100% brightness
}

// Set a specific miniature to selected mode (green color)
void LedMovementControl::setSelectedMode(int position) {
    ledControl.lightPosition(position, ledControl.getGreen());
    delay(500); 
    ledControl.lightPosition(position, ledControl.getRed());
    delay(500); 
    ledControl.lightPosition(position, ledControl.getBlue());
    delay(500); 
    ledControl.lightPosition(position, ledControl.getYellow());
    delay(500);
    ledControl.lightPosition(position, ledControl.getWhiteRGB());
    delay(500);
    ledControl.lightPosition(position, ledControl.getWhite(100));
}

void LedMovementControl::setAmbientAllLights(uint8_t brightnessPercentage) {
    pattern = Pattern::AmbientAll;
    isStandbyLight = true;
    ledControl.setWhite(brightnessPercentage);
}

void LedMovementControl::startAmbientRandom(uint8_t maxBrightnessPercentage, uint8_t density) {
    pattern = Pattern::AmbientRandom;
    isStandbyLight = true;

    ambientMaxBrightness = maxBrightnessPercentage;
    ambientDensity = density;
    lastAmbientUpdateMs = 0;

    for (int i = 0; i < NUM_LEDS; i++) {
        ambientLevels[i] = 0;
        ambientDelta[i] = 0;
    }

    ledControl.clearAll();

    // Seed randomness (best-effort)
    randomSeed(micros());
}

void LedMovementControl::stopAmbient() {
    if (pattern == Pattern::AmbientAll || pattern == Pattern::AmbientRandom) {
        pattern = Pattern::Focus;
        isStandbyLight = false;
    }
}

bool LedMovementControl::isAmbientActive() const {
    return pattern == Pattern::AmbientAll || pattern == Pattern::AmbientRandom;
}

void LedMovementControl::update() {
    if (pattern != Pattern::AmbientRandom) {
        return;
    }

    const unsigned long now = millis();
    // ~25 FPS max (lower refresh reduces chance of visual glitches)
    if (lastAmbientUpdateMs != 0 && (now - lastAmbientUpdateMs) < 40) {
        return;
    }
    lastAmbientUpdateMs = now;

    const uint8_t maxW = map(ambientMaxBrightness, 0, 100, 0, 255);
    const uint8_t step = 6;

    int active = 0;
    for (int i = 0; i < NUM_LEDS; i++) {
        if (ambientDelta[i] != 0) {
            active++;

            int next = static_cast<int>(ambientLevels[i]) + static_cast<int>(ambientDelta[i]);
            if (next >= maxW) {
                next = maxW;
                ambientDelta[i] = -static_cast<int8_t>(step);
            } else if (next <= 0) {
                next = 0;
                ambientDelta[i] = 0;
            }
            ambientLevels[i] = static_cast<uint8_t>(next);
        }
    }

    // Spawn new fades until we reach desired density
    const int target = ambientDensity;
    for (int tries = 0; active < target && tries < 10; tries++) {
        const int idx = random(NUM_LEDS);
        if (ambientDelta[idx] == 0 && ambientLevels[idx] == 0) {
            ambientDelta[idx] = static_cast<int8_t>(step);
            active++;
        }
    }

    for (int i = 0; i < NUM_LEDS; i++) {
        ledControl.setPixelWhite(i, ambientLevels[i]);
    }
    ledControl.show();
}