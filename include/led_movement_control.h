#ifndef LED_MOVEMENT_CONTROL_H
#define LED_MOVEMENT_CONTROL_H

#include "led_control.h"

class LedMovementControl {
public:
    // Constructor
    LedMovementControl(LedControl& ledControl);

    // Set all LEDs to standby mode (50% brightness)
    void setStandbyMode();

    // Highlight a specific miniature (focus mode)
    void setFocusMode(int position);

    // Set a specific miniature to selected mode (green color)
    void setSelectedMode(int position);

private:
    LedControl& ledControl;
};

#endif