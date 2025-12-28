#include <Arduino.h>
#include <ArduinoJson.h>
#include "config.h"
#include "led_control.h"
#include "tft_display_control.h" // Changed to TFT display
#include "encoder_control.h"
#include "nfc_reader_control.h"

// Create instances of our control classes
LedControl ledControl;
TFTDisplayControl displayControl; // Changed to TFTDisplayControl
EncoderControl encoderControl;
NFCReaderControl nfcReader;

// Current miniature index
int currentIndex = 0;

// Add a flag to track NFC reader connection
bool isNFCConnected = false;

void setup() {
  // Initialize serial communication
  Serial0.begin(115200);
  Serial0.println("Setup started...");

  // Initialize LED strip
  ledControl.begin();
  Serial0.println("LED strip initialized");

  // Initialize TFT display
  if (!displayControl.begin()) {
    Serial0.println("Failed to initialize TFT display!");
  } else {
    Serial0.println("TFT display initialized");
  }

  // Initialize the NFC reader conditionally
  if (nfcReader.begin()) {
    Serial0.println("NFC reader initialized");
    isNFCConnected = true;
  } else {
    Serial0.println("NFC reader not connected. NFC features disabled.");
    isNFCConnected = false;
  }
  
  // Initialize encoder
  encoderControl.begin();
  // Button Mode pin configuration
  pinMode(BTN_MODE, INPUT_PULLUP);
  
  // Set initial position
  currentIndex = 0;
  displayControl.showMiniatureInfo(currentIndex);
  ledControl.lightPosition(currentIndex, ledControl.getYellow());

}

void loop() {

  int modeBtnState = digitalRead(BTN_MODE);
  
  
  // Check for encoder movement
  if (encoderControl.checkMovement()) {
    // Update current index from encoder
    currentIndex = encoderControl.getCurrentIndex();
    
    // Update display with new miniature info
    displayControl.showMiniatureInfo(currentIndex);
    
    // Highlight the corresponding LED position
    ledControl.lightPosition(currentIndex, ledControl.getYellow());

    Serial0.print("Selected position: ");
    Serial0.println(currentIndex);
  }
  
  // Check for encoder button press
  if (encoderControl.isButtonPressed()) {
    Serial0.println("Button pressed!");

    // Confirm selection by changing color to green
    ledControl.lightPosition(currentIndex, ledControl.getGreen());

    delay(500);
    
    // Return to selected position
    ledControl.lightPosition(currentIndex, ledControl.getYellow());
  }

  
  // Execute NFC-related logic only if the NFC reader is connected
  if (isNFCConnected && modeBtnState == LOW) {
    Serial0.println("Mode button pressed!, Entering NFC read mode...");
    displayControl.showMode("NFC Read", "Reading tag...");

    uint8_t uid[7];
    uint8_t uidLength;
    JsonDocument jsonDoc;

    unsigned long startTime = millis();
    bool tagReadSuccessfully = false;
    bool tagReadError = false;

    while (millis() - startTime < 10000) { // 10-second timeout
      if (nfcReader.readTagUID(uid, uidLength)) {
        if (nfcReader.readTagContext(uid, uidLength, jsonDoc)) {
          tagReadSuccessfully = true;
          break; 
        } else {
          tagReadError = true;
          break;
        }
      } else {
        break;
      }
      delay(10);
    }

    if (tagReadSuccessfully) {
      displayControl.showInfo(
        jsonDoc["name"] | "Unknown",
        jsonDoc["team"] | "Unknown",
        jsonDoc["designBy"] | "Unknown",
        jsonDoc["painted"] | "Unknown"
      );
    } else if (tagReadError) {
      Serial0.println("Error reading NFC tag data.");
      displayControl.clear();
      displayControl.showMessage("Error reading tag!", 10, 100, 2, displayControl.getRedColor());
      delay(2000);
      displayControl.showMiniatureInfo(currentIndex);
    } else {
      Serial0.println("No NFC tag detected within timeout.");
      displayControl.clear();
      displayControl.showMessage("No tag detected!", 10, 100, 2, displayControl.getWhiteColor());
      delay(2000);
      displayControl.showMiniatureInfo(currentIndex);
    }
  }

    delay(10);
  
}

