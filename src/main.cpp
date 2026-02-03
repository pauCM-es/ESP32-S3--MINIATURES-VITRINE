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
#include "util/DeviceSettings.h"
#include "util/SettingsStore.h"

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
unsigned long lastActivityMs = 0;
bool inMenu = false;

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

  // Load persisted settings early (needed for WiFi init)
  DeviceSettings bootSettings;
  SettingsStore::load(bootSettings);

  // Initialize networking
  wifiManager.begin(bootSettings);
  webServer.begin(&modeManager);
  attachWsEventHandlers(*webServer.getWsServer(), ledControl, ledMovementControl, &modeManager);

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

  // Load persisted settings into the ModeManager and apply them to hardware
  modeManager.begin(&bootSettings);
  
  // Set initial position
  currentIndex = modeManager.getLastMiniatureIndex();
  encoderControl.setCurrentIndex(currentIndex);
  displayControl.showMiniatureInfo(currentIndex);
  ledMovementControl.setFocusMode(currentIndex);

  lastActivityMs = millis();

  // Set standby brightness
  // modeManager.setStandbyBrightness(50);
}

void loop() {
  // Flush deferred persistence (e.g., lastMiniatureIndex)
  modeManager.tick();

  // Network is handled asynchronously by ESPAsyncWebServer

  // Advance ambient animations (non-blocking)
  ledMovementControl.update();

  // Sleep handling: wake on any user input
  int modeBtnState = digitalRead(BTN_MODE);
  const bool modeBtnPressedEdge = (modeBtnState != lastModeBtnState) && (modeBtnState == LOW);

  if (modeManager.isSleeping()) {
    const bool encoderMoved = encoderControl.checkMovement();
    const bool encoderPressed = encoderControl.isButtonPressed();

    if (encoderMoved || encoderPressed || modeBtnPressedEdge) {
      currentIndex = encoderControl.getCurrentIndex();
      modeManager.wakeFromSleep(currentIndex);
      lastActivityMs = millis();

      // Prevent the wake press from also triggering menu entry
      lastModeBtnState = modeBtnState;
    } else {
      // Keep loop light while sleeping
      lastModeBtnState = modeBtnState;
      delay(20);
    }
    return;
  }

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

  // Auto-sleep after inactivity (if enabled)
  if (!inMenu) {
    const uint32_t timeoutMs = modeManager.getSleepTimeoutMs();
    if (timeoutMs > 0 && (millis() - lastActivityMs) > timeoutMs) {
      modeManager.enterSleep();
      lastModeBtnState = modeBtnState;
      return;
    }
  }

  // Detect button press (active LOW due to INPUT_PULLUP)
  if (modeBtnState != lastModeBtnState) {
    if (modeBtnState == LOW) {
      LOGI("btn", "BTN_MODE pressed");

      lastActivityMs = millis();

      // Pause ambient while navigating menus
      ledMovementControl.stopAmbient();

      inMenu = true;

      modeManager.selectMainMode(
        [&](int modeIndex) {
          if (modeIndex < 0) {
            // Cancel/back: restore focus UI
            currentIndex = encoderControl.getCurrentIndex();
            displayControl.showMiniatureInfo(currentIndex);
            ledMovementControl.setFocusMode(currentIndex);
            return;
          }

          // Sleep is a special top-level action
          if (modeManager.isSleepMode(modeIndex)) {
            modeManager.enterSleep();
            return;
          }

          LOGI("mode", "Selected mode %d: %s", modeIndex, modeManager.getModeName(modeIndex));
          modeManager.handleModeOptions(modeIndex);

          // If an ambient option was selected, keep ambient active (don't restore focus UI/LEDs)
          if (ledMovementControl.isAmbientActive()) {
            return;
          }

          // Sync and refresh after menu interaction
          currentIndex = encoderControl.getCurrentIndex();
          displayControl.showMiniatureInfo(currentIndex);
          ledMovementControl.setFocusMode(currentIndex);
        }
      );

      inMenu = false;
      lastActivityMs = millis();
    }
    lastModeBtnState = modeBtnState;
  }

  // Check for encoder movement
  if (encoderControl.checkMovement()) {
    lastActivityMs = millis();
    if (ledMovementControl.isAmbientActive()) {
      // Ignore focus updates while ambient is active
    } else {
      // Update current index from encoder
      currentIndex = encoderControl.getCurrentIndex();
      modeManager.setLastMiniatureIndex(static_cast<uint8_t>(currentIndex));

      // Update display with new miniature info
      displayControl.showMiniatureInfo(currentIndex);

      if (webServer.getWsServer()) {
        broadcastEncoderRotate(*webServer.getWsServer(), currentIndex);
        broadcastDisplayMiniature(*webServer.getWsServer(), currentIndex);
      }

      // Highlight the corresponding LED position
      ledMovementControl.setFocusMode(currentIndex,ledMovementControl.getIsStandbyLight() );

      LOGI("encoder", "Selected position: %d", currentIndex);
    }
  }
   if (encoderControl.isButtonPressed()) {
    lastActivityMs = millis();
    if (ledMovementControl.isAmbientActive()) {
      // Ignore selection mode while ambient is active
    } else {
      LOGI("encoder", "Button press detected! Selecting LED.");

      if (webServer.getWsServer()) {
        broadcastEncoderPress(*webServer.getWsServer(), currentIndex);
      }

      // Set selected mode
      ledMovementControl.setSelectedMode(currentIndex);

      delay(500);

      // Return to focus mode
      ledMovementControl.setFocusMode(currentIndex);
    }
  }
  


  delay(10);
}
