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
    
    Serial0.println("Encoder initialized");
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

// Check if the button has been pressed with debounce
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

// Add variables to track brightness adjustment
bool isIncreasing = true; // Last mode used: true for increase, false for decrease
unsigned long buttonPressStartTime = 0;
const unsigned long longPressDuration = 1000; // 1 second for long press

// Add variables to track button press duration
unsigned long pressStartTime = 0;
const unsigned long shortPressThreshold = 500; // 500ms for short press

// Check if the button is pressed (short press)
bool EncoderControl::isShortPress() {
    if (isButtonPressed()) {
        if (pressStartTime == 0) {
            pressStartTime = millis();
        }
    } else {
        if (pressStartTime > 0 && (millis() - pressStartTime < shortPressThreshold)) {
            pressStartTime = 0; // Reset press start time
            return true; // Short press detected
        }
        pressStartTime = 0; // Reset press start time
    }
    return false;
}

// Check if the button is pressed (long press)
bool EncoderControl::isLongPress() {
    if (isButtonPressed()) {
        if (pressStartTime == 0) {
            pressStartTime = millis();
        }
        if (millis() - pressStartTime >= shortPressThreshold) {
            return true; // Long press detected
        }
    } else {
        pressStartTime = 0; // Reset press start time
    }
    return false;
}

// Method to adjust brightness
void EncoderControl::adjustBrightness(LedControl& ledControl) {
    static uint8_t brightness = 128; // Start with medium brightness (0-255)
    static unsigned long lastBrightnessUpdate = 0; // Track the last update time
    const unsigned long brightnessUpdateInterval = 50; // Minimum interval in milliseconds

    if (isLongPress()) {
        // Adjust brightness only if the update interval has passed
        if (millis() - lastBrightnessUpdate > brightnessUpdateInterval) {
            if (isIncreasing) {
                brightness = min(brightness + 1, 255); // Increase brightness
            } else {
                brightness = max(brightness - 1, 0);   // Decrease brightness
            }

            // Apply the new brightness to the LED control
            ledControl.setBrightness(brightness);

            // Update the last brightness update time
            lastBrightnessUpdate = millis();
        }
    }
}
