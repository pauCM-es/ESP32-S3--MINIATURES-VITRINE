#include "EncoderControl.h"

static int wrapIndex(int idx, int max) {
    if (max <= 0) return 0;
    while (idx < 0) idx += max;
    while (idx >= max) idx -= max;
    return idx;
}

// Constructor
EncoderControl::EncoderControl() {
    encoder = new ESP32Encoder();
    lastPosition = 0;
    currentIndex = 0;
    buttonState = false;
    lastButtonReading = false;
    lastDebounceTime = 0;

    pressStartTime = 0;
    lastPressDuration = 0;
    longPressArmed = false;

    pendingPressEvent = false;
    pendingShortPressEvent = false;
    pendingLongPressEvent = false;
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
    const long newPosition = encoder->getCount() / 2;  // Divide by 2 for half-step encoding
    if (newPosition == lastPosition) {
        return false;
    }

    long delta = newPosition - lastPosition;
    // Clamp excessively large jumps (e.g., due to missed interrupts) to something sane.
    if (delta > 4) delta = 4;
    if (delta < -4) delta = -4;

    currentIndex = wrapIndex(currentIndex + static_cast<int>(delta), MAX_MINIATURES);
    lastPosition = newPosition;
    return true;
}

// Check if the button has been pressed with debounce
bool EncoderControl::isButtonPressed() {
    updateButton();
    const bool wasPressed = pendingPressEvent;
    pendingPressEvent = false;
    return wasPressed;
}

// Check if the button is pressed (short press)
bool EncoderControl::isShortPress() {
    updateButton();
    const bool wasShort = pendingShortPressEvent;
    pendingShortPressEvent = false;
    return wasShort;
}

// Check if the button is pressed (long press)
bool EncoderControl::isLongPress() {
    updateButton();
    const bool wasLong = pendingLongPressEvent;
    pendingLongPressEvent = false;
    return wasLong;
}

// Method to adjust brightness
void EncoderControl::adjustBrightness(LedControl& ledControl) {
    static uint8_t brightness = 128; // Start with medium brightness (0-255)
    static unsigned long lastBrightnessUpdate = 0; // Track the last update time
    const unsigned long brightnessUpdateInterval = 50; // Minimum interval in milliseconds

    static bool isIncreasing = true; // true for increase, false for decrease

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

void EncoderControl::updateButton() {
    // Raw reading: true when pressed (INPUT_PULLUP -> LOW when pressed)
    const bool reading = (digitalRead(ENCODER_BUTTON) == LOW);

    if (reading != lastButtonReading) {
        lastDebounceTime = millis();
        lastButtonReading = reading;
    }

    if ((millis() - lastDebounceTime) <= debounceDelay) {
        return;
    }

    // Debounced transition
    if (reading != buttonState) {
        buttonState = reading;
        if (buttonState) {
            // Press
            pressStartTime = millis();
            longPressArmed = true;
            pendingPressEvent = true;
        } else {
            // Release
            if (pressStartTime != 0) {
                lastPressDuration = millis() - pressStartTime;
            } else {
                lastPressDuration = 0;
            }

            if (lastPressDuration < longPressThresholdMs) {
                pendingShortPressEvent = true;
            }

            pressStartTime = 0;
            longPressArmed = false;
        }
    }

    // Long-press fires once per press
    if (buttonState && longPressArmed && pressStartTime != 0) {
        const unsigned long heldMs = millis() - pressStartTime;
        if (heldMs >= longPressThresholdMs) {
            pendingLongPressEvent = true;
            longPressArmed = false;
        }
    }
}
