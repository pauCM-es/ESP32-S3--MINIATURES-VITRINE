#ifndef LED_MOVEMENT_CONTROL_H
#define LED_MOVEMENT_CONTROL_H

#include "LedControl.h"

class LedMovementControl {
public:
    // Constructor
    LedMovementControl(LedControl& ledControl);

    // Set all LEDs to standby mode (50% brightness)
    void setStandbyMode(int brightness);

    // Highlight a specific miniature (focus mode)
    void setFocusMode(int position, bool lightUpRest = false);

    // Set a specific miniature to selected mode (green color)
    void setSelectedMode(int position);

    // Ambient patterns
    void setAmbientAllLights(uint8_t brightnessPercentage);
    void startAmbientRandom(uint8_t maxBrightnessPercentage, uint8_t density);
    void stopAmbient();
    bool isAmbientActive() const;

    // Call frequently from loop() to advance animations
    void update();

    // Turn off all LEDs immediately
    void clearAll();

    boolean getIsStandbyLight() {
        return isStandbyLight;
    }

private:
    LedControl& ledControl;
    boolean isStandbyLight = false;

    enum class Pattern {
        Focus,
        Standby,
        AmbientAll,
        AmbientRandom
    };

    Pattern pattern = Pattern::Focus;

    // Ambient random state
    unsigned long lastAmbientUpdateMs = 0;
    uint8_t ambientMaxBrightness = 60;
    uint8_t ambientDensity = 6;
    uint8_t ambientLevels[NUM_LEDS] = {0};
    int8_t ambientDelta[NUM_LEDS] = {0};
};

#endif