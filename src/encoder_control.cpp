#include "encoder_control.h"

// Constructor
EncoderControl::EncoderControl() {
    encoder = new ESP32Encoder();
    lastPosition = 0;
    currentIndex = 0;
    buttonState = false;
    lastButtonState = false;
    lastDebounceTime = 0;
}

// Initialize encoder
void EncoderControl::begin() {
    // Configure encoder pins
    ESP32Encoder::useInternalWeakPullResistors = UP;
    encoder->attachHalfQuad(ENCODER_PIN_A, ENCODER_PIN_B);
    encoder->setCount(0);
    
    // Configure button pin with pull-up resistor
    pinMode(ENCODER_BUTTON, INPUT_PULLUP);
    
    Serial.println("Encoder initialized");
}

// Get current selection index
int EncoderControl::getCurrentIndex() {
    return currentIndex;
}

// Check if encoder has moved and update index if needed
bool EncoderControl::checkMovement() {
    long newPosition = encoder->getCount() / 2;  // Divide by 2 for half-step encoding
    
    // Check if the encoder has moved
    if (newPosition != lastPosition) {
        // Calculate direction and change in index
        int direction = (newPosition > lastPosition) ? 1 : -1;
        currentIndex += direction;
        
        // Ensure index stays within bounds
        if (currentIndex < 0) {
            currentIndex = MAX_MINIATURES - 1;
        } else if (currentIndex >= MAX_MINIATURES) {
            currentIndex = 0;
        }
        lastPosition = newPosition;
        return true;  // Position has changed
    }
    
    return false;  // No change
}

// Check if button has been pressed with debounce
bool EncoderControl::isButtonPressed() {
    // Read the current state of the button (inverted because of pull-up)
    bool reading = !digitalRead(ENCODER_BUTTON);
    bool result = false;
    
    // Check if the button state has changed
    if (reading != lastButtonState) {
        lastDebounceTime = millis();
    }
    
    // If the button state has been stable for longer than the debounce delay
    if ((millis() - lastDebounceTime) > debounceDelay) {
        // If the button state has changed since the last reading
        if (reading != buttonState) {
            buttonState = reading;
            
            // Only consider the button press (not release)
            if (buttonState) {
                result = true;
            }
        }
    }
    
    lastButtonState = reading;
    return result;
}
