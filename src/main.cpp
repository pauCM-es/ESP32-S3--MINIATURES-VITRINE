#include <Arduino.h>
#include <ArduinoJson.h>
#include "config.h"
#include "led_control.h"
#include "tft_display_control.h" // Changed to TFT display
#include "encoder_control.h"
#include "nfc_reader_control.h"
#include "led_movement_control.h"
#include "mode_manager.h"

// Create instances of our control classes
LedControl ledControl;
TFTDisplayControl displayControl; // Changed to TFTDisplayControl
EncoderControl encoderControl;
NFCReaderControl nfcReader;

// Create an instance of LedMovementControl
LedMovementControl ledMovementControl(ledControl);

// Create an instance of ModeManager
ModeManager modeManager(ledMovementControl, nfcReader, displayControl);

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
  ledMovementControl.setFocusMode(currentIndex);

  // Set standby brightness
  modeManager.setStandbyBrightness(50);
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
    ledMovementControl.setFocusMode(currentIndex);

    Serial0.print("Selected position: ");
    Serial0.println(currentIndex);
  }
   if (encoderControl.isButtonPressed()) {
    Serial0.println("Button press detected! Selecting LED.");

    // Set selected mode
    ledMovementControl.setSelectedMode(currentIndex);

    delay(500);

    // Return to focus mode
    ledMovementControl.setFocusMode(currentIndex);
  }
  


  delay(10);
}

