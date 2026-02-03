#include "ModeManager.h"
#include "LedMovementControl.h"
#include "NfcControl.h"
#include "DisplayControl.h"
#include "EncoderControl.h"
#include "modes/ModesRegistry.h"
#include "util/SettingsStore.h"
#include <cstring>

ModeManager::ModeManager(LedMovementControl& ledMovementControl, NFCReaderControl& nfcReader, TFTDisplayControl& displayControl, EncoderControl& encoderControl)
    : ledMovementControl(ledMovementControl), nfcReader(nfcReader), displayControl(displayControl), encoderControl(encoderControl) {}

void ModeManager::begin() {
    SettingsStore::load(settings);
    applySettingsToHardware();
}

void ModeManager::applySettingsToHardware() {
    displayControl.setBacklightBrightnessPercent(settings.backlightBrightnessPercent);
    // Keep current on/off state as controlled by sleep
    if (!sleeping) {
        displayControl.setBacklight(true);
    }
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
   displayControl.showMode("Settings", "Reading tag...");
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

    selectMode(
        modeNames,
        numModes + 1,
        [&](int selectedIndex) {
            if (selectedIndex < 0 || selectedIndex >= numModes) {
                callback(-1);
                return;
            }
            callback(selectedIndex);
        },
        /*initialFocusIndex=*/numModes
    );
}

void ModeManager::selectMode(const char* const options[], int numOptions, std::function<void(int)> callback, int initialFocusIndex) {
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

    while (!optionSelected) {
        if (focusIndex != lastRenderedFocusIndex) {
            displayControl.showOptions(options, numOptions, focusIndex);
            lastRenderedFocusIndex = focusIndex;
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
        /*initialFocusIndex=*/mode.numOptions
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