#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <WebServer.h>
#include <LittleFS.h>
#include "config.h"
#include "log.h"
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
ModeManager modeManager(ledMovementControl, nfcReader, displayControl, encoderControl);

// Current miniature index
int currentIndex = 0;

// Add a flag to track NFC reader connection
bool isNFCConnected = false;

// Track last state of mode button for edge detection
int lastModeBtnState = HIGH;

static WebServer server(80);
static bool fsMounted = false;

#if __has_include("secrets.h")
#include "secrets.h"
#define HAS_WIFI_SECRETS 1
#else
#define HAS_WIFI_SECRETS 0
#endif

static const char *getContentType(const String &path) {
  if (path.endsWith(".html")) return "text/html";
  if (path.endsWith(".css")) return "text/css";
  if (path.endsWith(".js")) return "application/javascript";
  if (path.endsWith(".json")) return "application/json";
  if (path.endsWith(".png")) return "image/png";
  if (path.endsWith(".jpg") || path.endsWith(".jpeg")) return "image/jpeg";
  if (path.endsWith(".svg")) return "image/svg+xml";
  if (path.endsWith(".ico")) return "image/x-icon";
  return "text/plain";
}

static bool streamFileFromLittleFS(const String &path) {
  if (!fsMounted) {
    return false;
  }
  if (!LittleFS.exists(path)) {
    return false;
  }
  File file = LittleFS.open(path, "r");
  if (!file) {
    return false;
  }
  server.streamFile(file, getContentType(path));
  file.close();
  return true;
}

void setup() {
  // Initialize serial communication + logging
  Log::begin(Serial0, 115200);
  LOGI("boot", "Setup started...");
  LOGI("boot", "Build: %s %s", __DATE__, __TIME__);
  LOGI("boot", "SDK: %s", ESP.getSdkVersion());
  LOGI("boot", "CPU Freq: %u MHz", ESP.getCpuFreqMHz());

  // Step 3: mount LittleFS
  fsMounted = LittleFS.begin(true);
  if (!fsMounted) {
    LOGE("fs", "LittleFS mount failed");
  } else {
    LOGI("fs", "LittleFS mounted");
  }

  // Step 3: Wi-Fi + HTTP server
  WiFi.mode(WIFI_MODE_APSTA);
  if (HAS_WIFI_SECRETS) {
    LOGI("wifi", "Connecting to SSID: %s", WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    const uint32_t startMs = millis();
    while (WiFi.status() != WL_CONNECTED && (millis() - startMs) < 15000) {
      delay(250);
    }
    if (WiFi.status() == WL_CONNECTED) {
      LOGI("wifi", "Connected, IP: %s", WiFi.localIP().toString().c_str());
    } else {
      LOGW("wifi", "STA connect timeout; falling back to AP only");
    }
  }

  const char *apSsid = "Vitrine-ESP32S3";
  const char *apPass = "vitrine1234"; // WPA2 requires >=8 chars
  if (WiFi.softAP(apSsid, apPass)) {
    LOGI("wifi", "AP started: %s", apSsid);
    LOGI("wifi", "AP password: %s", apPass);
    LOGI("wifi", "AP IP: %s", WiFi.softAPIP().toString().c_str());
  } else {
    LOGW("wifi", "Failed to start AP");
  }

  server.on("/", HTTP_GET, []() {
    if (fsMounted && streamFileFromLittleFS("/index.html")) {
      return;
    }
    server.send(500, "text/plain", fsMounted ? "index.html missing in LittleFS" : "LittleFS not mounted");
  });

  server.on("/index.html", HTTP_GET, []() {
    if (fsMounted && streamFileFromLittleFS("/index.html")) {
      return;
    }
    server.send(404, "text/plain", fsMounted ? "Not found" : "LittleFS not mounted");
  });

  server.onNotFound([]() {
    // Step 3: serve static files only for direct hits (no SPA fallback yet)
    const String path = server.uri();
    if (fsMounted && streamFileFromLittleFS(path)) {
      return;
    }
    server.send(404, "text/plain", "Not found");
  });

  server.begin();
  LOGI("http", "HTTP server started on port 80");

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
  server.handleClient();

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

