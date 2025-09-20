#include <Arduino.h>
#include "config.h"
#include "led_control.h"
#include "tft_display_control.h" // Changed to TFT display
#include "encoder_control.h"

// Create instances of our control classes
LedControl ledControl;
TFTDisplayControl displayControl; // Changed to TFTDisplayControl
EncoderControl encoderControl;


// Current miniature index
int currentIndex = 0;

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  Serial.println("Setup started...");

  // Initialize LED strip
  ledControl.begin();
  Serial.println("LED strip initialized");

  // Initialize TFT display
  if (!displayControl.begin()) {
    Serial.println("Failed to initialize TFT display!");
  } else {
    Serial.println("TFT display initialized");
  }
  
  // Initialize encoder
  encoderControl.begin();
  
  // Set initial position
  currentIndex = 0;
  displayControl.showMiniatureInfo(currentIndex);
  ledControl.lightPosition(currentIndex, ledControl.getYellow());

}

void loop() {
  // Check for encoder movement
  if (encoderControl.checkMovement()) {
    // Update current index from encoder
    currentIndex = encoderControl.getCurrentIndex();
    
    // Update display with new miniature info
    displayControl.showMiniatureInfo(currentIndex);
    
    // Highlight the corresponding LED position
    ledControl.lightPosition(currentIndex, ledControl.getYellow());

    Serial.print("Selected position: ");
    Serial.println(currentIndex);
  }
  
  // Check for encoder button press
  if (encoderControl.isButtonPressed()) {
    Serial.println("Button pressed!");

    // Confirm selection by changing color to green
    ledControl.lightPosition(currentIndex, ledControl.getGreen());

    delay(500);
    
    // Return to selected position
    ledControl.lightPosition(currentIndex, ledControl.getYellow());
  }
  
  // Small delay to prevent CPU hogging
  delay(10);
}
