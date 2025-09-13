#include <Arduino.h>
#include "config.h"
#include "led_control.h"
#include "display_control.h"
#include "encoder_control.h"

// Pin connections for ESP32-S3:
// - LED Strip: GPIO48
// - OLED Display: SDA=GPIO8, SCL=GPIO9
// - Rotary Encoder: A=GPIO15, B=GPIO16, Button=GPIO14
// - Mode Button: GPIO17

// Create instances of our control classes
LedControl ledControl;
DisplayControl displayControl;
EncoderControl encoderControl;

// Current miniature index
int currentIndex = 0;

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  Serial.println("ESP32-S3 Miniatures Vitrine - Phase 1 & 2");
  Serial.println("Configuration: LED=GPIO48, OLED=I2C(SDA:8,SCL:9), Encoder=(15,16,14)");
  
  // Initialize LED strip
  ledControl.begin();
  Serial.println("LED strip initialized");
  
  // Initialize OLED display
  if (!displayControl.begin()) {
    Serial.println("Failed to initialize display!");
  } else {
    Serial.println("OLED display initialized");
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
