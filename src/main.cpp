#include <Arduino.h>
#include "config.h"
#include "led_control.h"
#include "tft_display_control.h" // Changed to TFT display
#include "encoder_control.h"

// Pin connections for ESP32-S3:
// - LED Strip: GPIO47
// - TFT Display: CS=GPIO10, DC=GPIO14, RST=GPIO18, SCK=GPIO12 (HW SPI), MOSI=GPIO11 (HW SPI), BLK=GPIO19
// - Rotary Encoder: A=GPIO15, B=GPIO16, Button=GPIO17
// - Mode Button: GPIO9

// Create instances of our control classes
LedControl ledControl;
TFTDisplayControl displayControl; // Changed to TFTDisplayControl
EncoderControl encoderControl;

// Current miniature index
int currentIndex = 0;

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  Serial.println("ESP32-S3 Miniatures Vitrine - Phase 1 & 2");
  Serial.println("Configuration: LED=GPIO47, Encoder=(15,16,17)");
  Serial.println("TFT Display: CS=10, DC=14, RST=18, SCK=12, MOSI=11, BLK=19 (Hardware SPI)");
  
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
