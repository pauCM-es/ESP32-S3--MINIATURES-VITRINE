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

Adafruit_NeoPixel ledStrip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

// Current miniature index
int currentIndex = 0;

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  Serial.println("Setup started...");
  // Initialize LED strip
  // ledControl.begin();
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
  // ledControl.lightPosition(currentIndex, ledControl.getYellow());

  ledStrip.begin();
  ledStrip.setBrightness(150);
  ledStrip.setPixelColor(currentIndex, ledStrip.Color(255, 0, 0)); // Establecer en rojo
  ledStrip.show();
}

void loop() {
  Serial.println("Loop iteration...");
  // delay(5000);
  // Check for encoder movement
  if (encoderControl.checkMovement()) {
    // Update current index from encoder
    currentIndex = encoderControl.getCurrentIndex();
    
    // Update display with new miniature info
    displayControl.showMiniatureInfo(currentIndex);
    
    // Highlight the corresponding LED position
    // ledControl.lightPosition(currentIndex, ledControl.getYellow());
    ledStrip.clear();
    ledStrip.setPixelColor(currentIndex, ledStrip.Color(255, 0, 0));
    ledStrip.show();
    Serial.print("Selected position: ");
    Serial.println(currentIndex);
  }
  
  // Check for encoder button press
  if (encoderControl.isButtonPressed()) {
    Serial.println("Button pressed!");
    ledStrip.setPixelColor(currentIndex, ledStrip.Color(0, 0, 255));
    ledStrip.show();
    // // Confirm selection by changing color to green
    // ledControl.lightPosition(currentIndex, ledControl.getGreen());
    delay(1000);
     ledStrip.setPixelColor(currentIndex, ledStrip.Color(255, 0, 0));
    ledStrip.show();
    
    // // Return to selected position
    // ledControl.lightPosition(currentIndex, ledControl.getYellow());
  }
  
  // Small delay to prevent CPU hogging
  delay(10);
}
