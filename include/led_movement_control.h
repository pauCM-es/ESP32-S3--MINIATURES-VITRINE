#ifndef LED_MOVEMENT_CONTROL_H
#define LED_MOVEMENT_CONTROL_H

#include "led_control.h"

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

    boolean getIsStandbyLight() {
        return isStandbyLight;
    }

private:
    LedControl& ledControl;
    boolean isStandbyLight = false;
};

#endif