#include "ModeManager.h"
#include "LedMovementControl.h"
#include "NfcControl.h"
#include "DisplayControl.h"
#include "EncoderControl.h"
#include "config.h"
#include "modes/ModesRegistry.h"
#include "util/SettingsStore.h"
#include <cstring>
#include <WiFi.h>
#include "esp_sleep.h"
#include "driver/gpio.h"

ModeManager::ModeManager(LedMovementControl& ledMovementControl, NFCReaderControl& nfcReader, TFTDisplayControl& displayControl, EncoderControl& encoderControl)
    : ledMovementControl(ledMovementControl), nfcReader(nfcReader), displayControl(displayControl), encoderControl(encoderControl) {}

void ModeManager::begin(const DeviceSettings* initialSettings) {
    if (initialSettings) {
        settings = *initialSettings;
    } else {
        SettingsStore::load(settings);
    }
    applySettingsToHardware();
}

void ModeManager::tick() {
    if (!pendingSave) {
        return;
    }

    const unsigned long now = millis();
    if (pendingSaveDueMs != 0 && now < pendingSaveDueMs) {
        return;
    }

    persistSettings();
    pendingSave = false;
    pendingSaveDueMs = 0;
}

uint8_t ModeManager::getLastMiniatureIndex() const {
    return settings.lastMiniatureIndex;
}

void ModeManager::setLastMiniatureIndex(uint8_t index) {
    if (index >= MAX_MINIATURES) {
        index = 0;
    }
    if (settings.lastMiniatureIndex == index) {
        return;
    }
    settings.lastMiniatureIndex = index;

    // Defer persistence to avoid flash wear while rotating encoder.
    pendingSave = true;
    pendingSaveDueMs = millis() + 1500;
}

void ModeManager::applySettingsToHardware() {
    displayControl.setBacklightBrightnessPercent(settings.backlightBrightnessPercent);
    // Keep current on/off state as controlled by sleep
    if (!sleeping) {
        displayControl.setBacklight(true);
    }

    ledMovementControl.setLedBrightnessPercent(settings.ledBrightnessPercent);
    ledMovementControl.setStandbyBrightnessPercent(settings.standbyBrightnessPercent);

    ledMovementControl.setAmbientRandomSpeed(settings.ambientRandomFrameMs, settings.ambientRandomStep);
}

bool ModeManager::persistSettings() {
    return SettingsStore::save(settings);
}

void ModeManager::addNewMiniature() {
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
        displayControl.showMessage("Error reading tag!", 10, 100, 2, displayControl.getRedColor());
    } else {
        displayControl.showMessage("No tag detected!", 10, 100, 2, displayControl.getWhiteColor());
    }
}

void ModeManager::setStandbyBrightness(uint8_t brightness) {
    setStandbyBrightnessPercent(brightness);
}

void ModeManager::showStatus(const char* title, const char* message) {
    displayControl.showMode(title, message);
}

void ModeManager::ambientAllLights() {
    ledMovementControl.setAmbientAllLights(settings.ambientAllLightsBrightnessPercent);
    showStatus("Ambient", "All lights");
}

void ModeManager::ambientRandom() {
    ledMovementControl.setAmbientRandomSpeed(settings.ambientRandomFrameMs, settings.ambientRandomStep);
    ledMovementControl.startAmbientRandom(settings.ambientRandomMaxBrightnessPercent, settings.ambientRandomDensity);
    showStatus("Ambient", "Random");
}

void ModeManager::setBacklightBrightnessPercent(uint8_t percent) {
    if (percent > 100) {
        percent = 100;
    }
    settings.backlightBrightnessPercent = percent;
    displayControl.setBacklightBrightnessPercent(percent);
    persistSettings();
}

uint8_t ModeManager::getBacklightBrightnessPercent() const {
    return settings.backlightBrightnessPercent;
}

void ModeManager::setLedBrightnessPercent(uint8_t percent) {
    if (percent > 100) {
        percent = 100;
    }
    settings.ledBrightnessPercent = percent;
    ledMovementControl.setLedBrightnessPercent(percent);
    persistSettings();
}

uint8_t ModeManager::getLedBrightnessPercent() const {
    return settings.ledBrightnessPercent;
}

void ModeManager::setStandbyBrightnessPercent(uint8_t percent) {
    if (percent > 100) {
        percent = 100;
    }
    settings.standbyBrightnessPercent = percent;
    ledMovementControl.setStandbyBrightnessPercent(percent);
    persistSettings();
}

uint8_t ModeManager::getStandbyBrightnessPercent() const {
    return settings.standbyBrightnessPercent;
}

void ModeManager::setAmbientAllLightsBrightnessPercent(uint8_t percent) {
    if (percent > 100) {
        percent = 100;
    }
    settings.ambientAllLightsBrightnessPercent = percent;
    persistSettings();
}

uint8_t ModeManager::getAmbientAllLightsBrightnessPercent() const {
    return settings.ambientAllLightsBrightnessPercent;
}

void ModeManager::setAmbientRandomMaxBrightnessPercent(uint8_t percent) {
    if (percent > 100) {
        percent = 100;
    }
    settings.ambientRandomMaxBrightnessPercent = percent;
    persistSettings();
}

uint8_t ModeManager::getAmbientRandomMaxBrightnessPercent() const {
    return settings.ambientRandomMaxBrightnessPercent;
}

void ModeManager::setAmbientRandomDensity(uint8_t density) {
    if (density == 0) {
        density = 1;
    }
    settings.ambientRandomDensity = density;
    persistSettings();
}

uint8_t ModeManager::getAmbientRandomDensity() const {
    return settings.ambientRandomDensity;
}

void ModeManager::setAmbientRandomSpeed(uint16_t frameMs, uint8_t step) {
    if (frameMs == 0) {
        frameMs = 40;
    }
    if (step == 0) {
        step = 6;
    }
    settings.ambientRandomFrameMs = frameMs;
    settings.ambientRandomStep = step;
    ledMovementControl.setAmbientRandomSpeed(frameMs, step);
    persistSettings();
}

uint16_t ModeManager::getAmbientRandomFrameMs() const {
    return settings.ambientRandomFrameMs;
}

uint8_t ModeManager::getAmbientRandomStep() const {
    return settings.ambientRandomStep;
}

bool ModeManager::resetPersistedSettings() {
    const bool ok = SettingsStore::reset();
    settings = DeviceSettings{};
    applySettingsToHardware();
    return ok;
}

bool ModeManager::getWifiStaEnabled() const {
    return settings.wifiStaEnabled;
}

const char* ModeManager::getWifiStaSsid() const {
    return settings.wifiStaSsid;
}

bool ModeManager::hasWifiStaPass() const {
    return settings.wifiStaPass[0] != '\0';
}

const char* ModeManager::getWifiApSsid() const {
    return settings.wifiApSsid;
}

bool ModeManager::hasWifiApPass() const {
    return settings.wifiApPass[0] != '\0';
}

namespace {
bool copyCStrIfChanged(char* dst, size_t dstSize, const char* src) {
    if (!dst || dstSize == 0) {
        return false;
    }
    if (!src) {
        src = "";
    }

    // Fast path: unchanged
    if (strncmp(dst, src, dstSize) == 0) {
        // Note: strncmp() stops at '\0'; safe for comparing to src.
        return false;
    }

    strncpy(dst, src, dstSize - 1);
    dst[dstSize - 1] = '\0';
    return true;
}
}

bool ModeManager::setWifiStaConfig(bool enabled, const char* ssid, const char* pass, bool passProvided) {
    bool changed = false;

    if (settings.wifiStaEnabled != enabled) {
        settings.wifiStaEnabled = enabled;
        changed = true;
    }

    if (ssid != nullptr) {
        changed |= copyCStrIfChanged(settings.wifiStaSsid, sizeof(settings.wifiStaSsid), ssid);
    }

    if (passProvided) {
        changed |= copyCStrIfChanged(settings.wifiStaPass, sizeof(settings.wifiStaPass), pass);
    }

    if (changed) {
        persistSettings();
    }
    return changed;
}

bool ModeManager::setWifiApConfig(const char* ssid, const char* pass, bool passProvided) {
    bool changed = false;

    if (ssid != nullptr) {
        changed |= copyCStrIfChanged(settings.wifiApSsid, sizeof(settings.wifiApSsid), ssid);
    }

    if (passProvided) {
        changed |= copyCStrIfChanged(settings.wifiApPass, sizeof(settings.wifiApPass), pass);
    }

    if (changed) {
        persistSettings();
    }
    return changed;
}

int ModeManager::getNumModes() const {
    return Modes::getNumModes();
}

const char* ModeManager::getModeName(int modeIndex) const {
    return Modes::getMode(modeIndex).name;
}

void ModeManager::selectMainMode(std::function<void(int)> callback) {
    const int numModes = getNumModes();
    const int maxModes = Modes::MAX_MODES;
    if (numModes <= 0) {
        return;
    }

    if (numModes > maxModes) {
        displayControl.showMode("Error", "Too many modes");
        return;
    }

    // Add a "Back" entry so users can exit without triggering a mode.
    const char* modeNames[Modes::MAX_MODES + 1];
    for (int i = 0; i < numModes; i++) {
        modeNames[i] = getModeName(i);
    }
    modeNames[numModes] = "Back";

    int initialFocus = numModes;
    if (settings.lastMainModeIndex >= 0 && settings.lastMainModeIndex < numModes) {
        initialFocus = settings.lastMainModeIndex;
    }

    selectMode(
        modeNames,
        numModes + 1,
        [&](int selectedIndex) {
            if (selectedIndex < 0 || selectedIndex >= numModes) {
                callback(-1);
                return;
            }

            settings.lastMainModeIndex = static_cast<int8_t>(selectedIndex);
            persistSettings();

            callback(selectedIndex);
        },
        /*initialFocusIndex=*/initialFocus
    );
}

void ModeManager::selectMode(const char* const options[], int numOptions, std::function<void(int)> callback, int initialFocusIndex, int selectedIndex, const char* footerHint) {
    if (numOptions <= 0) {
        return;
    }

    const int savedMiniatureIndex = encoderControl.getCurrentIndex();
    // Default focus can be set by caller (e.g. "Back")
    if (initialFocusIndex < 0) initialFocusIndex = 0;
    if (initialFocusIndex >= numOptions) initialFocusIndex = numOptions - 1;
    encoderControl.setCurrentIndex(initialFocusIndex);

    int focusIndex = initialFocusIndex;
    int lastRenderedFocusIndex = -1;
    bool optionSelected = false;

    // Allow using BTN_MODE as a quick "Back" while in menus.
    int lastModeBtnState = digitalRead(BTN_MODE);

    while (!optionSelected) {
        if (focusIndex != lastRenderedFocusIndex) {
            displayControl.showOptions(options, numOptions, focusIndex, selectedIndex, footerHint);
            lastRenderedFocusIndex = focusIndex;
        }

        const int modeBtnState = digitalRead(BTN_MODE);
        const bool modeBtnPressedEdge = (modeBtnState != lastModeBtnState) && (modeBtnState == LOW);
        lastModeBtnState = modeBtnState;
        if (!optionSelected && modeBtnPressedEdge) {
            encoderControl.setCurrentIndex(savedMiniatureIndex);
            callback(-1);
            optionSelected = true;
            break;
        }

        if (encoderControl.checkMovementWithWrap(numOptions)) {
            focusIndex = encoderControl.getCurrentIndex();
        }

        if (encoderControl.isShortPress()) {
            const int selectedIndex = focusIndex;
            encoderControl.setCurrentIndex(savedMiniatureIndex);
            callback(selectedIndex);
            optionSelected = true;
        }

        // Long press cancels the menu
        if (!optionSelected && encoderControl.isLongPress()) {
            encoderControl.setCurrentIndex(savedMiniatureIndex);
            callback(-1);
            optionSelected = true;
        }

        delay(10);
    }

    // Safety: ensure we restore even if the loop exits unexpectedly.
    encoderControl.setCurrentIndex(savedMiniatureIndex);
}

void ModeManager::handleModeOptions(int modeIndex) {
    const Modes::ModeDef& mode = Modes::getMode(modeIndex);
    if (mode.numOptions <= 0) {
        return;
    }

    // Add a "Back" option so exiting doesn't run an action.
    const char* optionsWithBack[Modes::MAX_MODE_OPTIONS + 1];
    for (int i = 0; i < mode.numOptions; i++) {
        optionsWithBack[i] = mode.options[i];
    }
    optionsWithBack[mode.numOptions] = "Back";

    const char* footerHint = nullptr;
    if (mode.name && (strcmp(mode.name, "Settings") == 0)) {
        footerHint = "Rotate: move  Press: select  MODE: back";
    }

    selectMode(
        optionsWithBack,
        mode.numOptions + 1,
        [&](int optionIndex) {
            if (optionIndex < 0 || optionIndex >= mode.numOptions) {
                return;
            }
            if (mode.actions[optionIndex]) {
                mode.actions[optionIndex](*this);
            }
        },
        /*initialFocusIndex=*/mode.numOptions,
        /*selectedIndex=*/-1,
        footerHint
    );
}

void ModeManager::enterSleep() {
    if (sleeping) {
        return;
    }

    sleeping = true;
    ledMovementControl.stopAmbient();
    ledMovementControl.clearAll();
    displayControl.setBacklight(false);
}

void ModeManager::wakeFromSleep(int currentIndex) {
    if (!sleeping) {
        return;
    }

    sleeping = false;
    displayControl.setBacklightBrightnessPercent(settings.backlightBrightnessPercent);
    displayControl.setBacklight(true);
    displayControl.showMiniatureInfo(currentIndex);
    ledMovementControl.setFocusMode(currentIndex);
}

bool ModeManager::isSleeping() const {
    return sleeping;
}

void ModeManager::setSleepTimeoutMinutes(uint16_t minutes) {
    settings.sleepTimeoutMinutes = minutes;
    persistSettings();
}

uint16_t ModeManager::getSleepTimeoutMinutes() const {
    return settings.sleepTimeoutMinutes;
}

uint32_t ModeManager::getSleepTimeoutMs() const {
    if (settings.sleepTimeoutMinutes == 0) {
        return 0;
    }
    return static_cast<uint32_t>(settings.sleepTimeoutMinutes) * 60UL * 1000UL;
}

bool ModeManager::isSleepMode(int modeIndex) const {
    const char* name = getModeName(modeIndex);
    return name && (strcmp(name, "Sleep") == 0);
}

[[noreturn]] void ModeManager::powerOffDeepSleep() {
    // Best-effort shutdown of peripherals before deep sleep.
    ledMovementControl.stopAmbient();
    ledMovementControl.clearAll();

    // Make sure the TFT isn't displaying full white if BL is on.
    displayControl.fillScreen(displayControl.getBlackColor());
    displayControl.setBacklight(false);

#ifdef TFT_BLK
    // Backlight is PWM-controlled (LEDC). In deep sleep the peripheral can stop,
    // so we forcibly drive the pin LOW and hold it there.
    ledcDetachPin(TFT_BLK);
    pinMode(TFT_BLK, OUTPUT);
    digitalWrite(TFT_BLK, LOW);
    gpio_hold_en(static_cast<gpio_num_t>(TFT_BLK));
    gpio_deep_sleep_hold_en();
#endif

    // Stop WiFi to reduce current before sleeping.
    WiFi.disconnect(true);
    WiFi.mode(WIFI_MODE_NULL);

    // No wake sources: behaves like "off" until reset/power-cycle.
    esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_ALL);
    esp_deep_sleep_start();

    // Should never reach here.
    while (true) {
        delay(1000);
    }
}