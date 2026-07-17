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
    void setAmbientRandomSpeed(uint16_t frameMs, uint8_t step);
    uint16_t getAmbientRandomFrameMs() const { return ambientFrameMs; }
    uint8_t getAmbientRandomStep() const { return ambientStep; }
    void stopAmbient();
    bool isAmbientActive() const;

    // Call frequently from loop() to advance animations
    void update();

    // Turn off all LEDs immediately
    void clearAll();

    // --- Segment management ---
    // Returns the first LED index of the given miniature position.
    int getSegmentStart(int position) const;

    // Returns the number of LEDs in the segment for the given position.
    int getSegmentSize(int position) const;

    // Override the segment size for a single position (1..NUM_LEDS).
    void setSegmentSize(int position, uint8_t numLeds);

    // Bulk-load segment sizes from a DeviceSettings array (called on begin/settings apply).
    void loadSegmentSizes(const uint8_t* sizes, int count);

    // Persisted brightness knobs
    void setLedBrightnessPercent(uint8_t percent);
    uint8_t getLedBrightnessPercent() const { return ledBrightnessPercent; }

    void setStandbyBrightnessPercent(uint8_t percent);
    uint8_t getStandbyBrightnessPercent() const { return standbyBrightnessPercent; }

    boolean getIsStandbyLight() {
        return isStandbyLight;
    }

private:
    LedControl& ledControl;
    boolean isStandbyLight = false;

    uint8_t ledBrightnessPercent = 40;
    uint8_t standbyBrightnessPercent = 30;

    // Per-miniature segment sizes (number of LEDs per position).
    uint8_t segmentLeds[MAX_MINIATURES];

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
    uint16_t ambientFrameMs = 40;
    uint8_t ambientStep = 6;
    uint8_t ambientLevels[NUM_LEDS] = {0};
    int8_t ambientDelta[NUM_LEDS] = {0};
};

#endif