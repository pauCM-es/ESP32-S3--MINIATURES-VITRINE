#include "LedMovementControl.h"
#include "LedControl.h"

// Constructor
LedMovementControl::LedMovementControl(LedControl& ledControl) : ledControl(ledControl) {}

// Set all LEDs to standby mode (50% brightness)
void LedMovementControl::setStandbyMode(int brightness) {
    ledControl.setWhite(brightness);
}

// Highlight a specific miniature (focus mode)
void LedMovementControl::setFocusMode(int position, bool lightUpRest) {
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