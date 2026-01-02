#ifndef ENCODER_CONTROL_H
#define ENCODER_CONTROL_H

#include <Arduino.h>
#include <ESP32Encoder.h>
#include "config.h"
#include "led_control.h"

class EncoderControl {
private:
    ESP32Encoder* encoder;
    int lastPosition;
    int currentIndex;
    bool buttonState;
    bool lastButtonState;
    unsigned long lastDebounceTime;
    static const unsigned long debounceDelay = 50;
    
public:
    // Constructor
    EncoderControl();
    
    // Initialize encoder
    void begin();
    
    // Get current selection index
    int getCurrentIndex();
    
    // Check if encoder has moved and update index if needed
    bool checkMovement();
    
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
