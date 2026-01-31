#include <Arduino.h>
#include "config.h"
#include "util/Log.h"
#include "net/WifiManager.h"
#include "net/WebServer.h"
#include "net/MaintenanceMode.h"
#include "hardware/LedControl.h"
#include "hardware/DisplayControl.h"
#include "hardware/EncoderControl.h"
#include "hardware/NfcControl.h"
#include "hardware/LedMovementControl.h"
#include "hardware/ModeManager.h"
#include "net/WsEventHandlers.h"

// Network managers
WifiManager wifiManager;
WebServer webServer;

// Hardware control instances
LedControl ledControl;
TFTDisplayControl displayControl;
EncoderControl encoderControl;
NFCReaderControl nfcReader;

// Movement and mode control
LedMovementControl ledMovementControl(ledControl);
ModeManager modeManager(ledMovementControl, nfcReader, displayControl, encoderControl);

// State tracking
int currentIndex = 0;
bool isNFCConnected = false;
int lastModeBtnState = HIGH;
bool lastMaintenanceActive = false;

void setup() {
  // Initialize serial communication + logging
  Log::begin(Serial0, 115200);
  LOGI("boot", "Setup started...");
  LOGI("boot", "Build: %s %s", __DATE__, __TIME__);
  LOGI("boot", "SDK: %s", ESP.getSdkVersion());
  LOGI("boot", "CPU Freq: %u MHz", ESP.getCpuFreqMHz());

  // Step 7: Maintenance mode boot trigger (optional)
  if (MaintenanceMode::checkBootTrigger()) {
    MaintenanceMode::getInstance().enter();
  }

  // Initialize networking
  wifiManager.begin();
  webServer.begin();
  attachWsEventHandlers(*webServer.getWsServer(), ledControl, ledMovementControl);

  // Initialize LED strip
  ledControl.begin();
  LOGI("led", "LED strip initialized");

  // Initialize TFT display
  if (!displayControl.begin()) {
    LOGE("display", "Failed to initialize TFT display!");
  } else {
    LOGI("display", "TFT display initialized");
  }

  // Initialize the NFC reader conditionally
  if (nfcReader.begin()) {
    LOGI("nfc", "NFC reader initialized");
    isNFCConnected = true;
  } else {
    LOGW("nfc", "NFC reader not connected. NFC features disabled.");
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
  // modeManager.setStandbyBrightness(50);
}

void loop() {
  // Network is handled asynchronously by ESPAsyncWebServer

  const bool maintenanceActive = MaintenanceMode::getInstance().isActive();
  if (maintenanceActive) {
    if (!lastMaintenanceActive) {
      LOGW("maint", "Maintenance mode active: pausing hardware actions");
      webServer.getWsServer()->closeAll();
      ledControl.clearAll();
    }
    lastMaintenanceActive = true;
    delay(50);
    return;
  }

  lastMaintenanceActive = false;
  
  int modeBtnState = digitalRead(BTN_MODE);
  // Detect button press (active LOW due to INPUT_PULLUP)
  if (modeBtnState != lastModeBtnState) {
    if (modeBtnState == LOW) {
      LOGI("btn", "BTN_MODE pressed");
      const boolean isStandby = ledMovementControl.getIsStandbyLight();
      ledMovementControl.setFocusMode(currentIndex, !isStandby);
      
    }
    lastModeBtnState = modeBtnState;
  }

  // Check for encoder movement
  if (encoderControl.checkMovement()) {
    // Update current index from encoder
    currentIndex = encoderControl.getCurrentIndex();

    // Update display with new miniature info
    displayControl.showMiniatureInfo(currentIndex);

    // Highlight the corresponding LED position
    ledMovementControl.setFocusMode(currentIndex,ledMovementControl.getIsStandbyLight() );

    LOGI("encoder", "Selected position: %d", currentIndex);
  }
   if (encoderControl.isButtonPressed()) {
    LOGI("encoder", "Button press detected! Selecting LED.");

    // Set selected mode
    ledMovementControl.setSelectedMode(currentIndex);

    delay(500);

    // Return to focus mode
    ledMovementControl.setFocusMode(currentIndex);
  }
  


  delay(10);
}
