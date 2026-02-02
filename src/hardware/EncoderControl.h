#ifndef ENCODER_CONTROL_H
#define ENCODER_CONTROL_H

#include <Arduino.h>
#include <ESP32Encoder.h>
#include "config.h"
#include "LedControl.h"

class EncoderControl {
private:
    ESP32Encoder* encoder;
    long lastPosition;
    int currentIndex;
    bool buttonState;       // debounced state (true = pressed)
    bool lastButtonReading; // raw reading (true = pressed)
    unsigned long lastDebounceTime;
    unsigned long pressStartTime;
    unsigned long lastPressDuration;
    bool longPressArmed;
    bool pendingPressEvent;
    bool pendingShortPressEvent;
    bool pendingLongPressEvent;
    static const unsigned long debounceDelay = 50;
    static const unsigned long longPressThresholdMs = 700;

    void updateButton();
    
public:
    // Constructor
    EncoderControl();
    
    // Initialize encoder
    void begin();
    
    // Get current selection index
    int getCurrentIndex();

    // Set current selection index (does not affect physical encoder count tracking)
    void setCurrentIndex(int index);
    
    // Check if encoder has moved and update index if needed
    bool checkMovement();

    // Check if encoder has moved and update index with a custom wrap range
    bool checkMovementWithWrap(int wrapMax);
    
    // Check if button has been pressed
    bool isButtonPressed();

    // Check if the button is pressed (short press)
    bool isShortPress();

    // Check if the button is pressed (long press)
    bool isLongPress();

    // Method to adjust brightness based on button press
    void adjustBrightness(LedControl& ledControl);
};

#endif // ENCODER_CONTROL_H
