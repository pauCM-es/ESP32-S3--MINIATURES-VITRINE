#include "ModeManager.h"
#include "LedMovementControl.h"
#include "NfcControl.h"
#include "DisplayControl.h"
#include "EncoderControl.h"

ModeManager::ModeManager(LedMovementControl& ledMovementControl, NFCReaderControl& nfcReader, TFTDisplayControl& displayControl, EncoderControl& encoderControl)
    : ledMovementControl(ledMovementControl), nfcReader(nfcReader), displayControl(displayControl), encoderControl(encoderControl) {}

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

void ModeManager::selectMainMode(std::function<void(int)> callback) {
    const char* modeNames[NUM_MODES];
    for (int i = 0; i < NUM_MODES; i++) {
        modeNames[i] = MODES[i].name;
    }

    selectMode(modeNames, NUM_MODES, callback);
}

void ModeManager::selectMode(const char* const options[], int numOptions, std::function<void(int)> callback) {
    if (numOptions <= 0) {
        return;
    }

    const int savedMiniatureIndex = encoderControl.getCurrentIndex();
    encoderControl.setCurrentIndex(0);

    int focusIndex = 0;
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

        delay(10);
    }

    // Safety: ensure we restore even if the loop exits unexpectedly.
    encoderControl.setCurrentIndex(savedMiniatureIndex);
}

void ModeManager::handleModeOptions(int modeIndex) {
    const Mode& mode = MODES[modeIndex];

    if (mode.numOptions <= 0) {
        return;
    }

    const int savedMiniatureIndex = encoderControl.getCurrentIndex();
    encoderControl.setCurrentIndex(0);

    int focusIndex = 0;
    int lastRenderedFocusIndex = -1;
    bool optionSelected = false;

    while (!optionSelected) {
        if (focusIndex != lastRenderedFocusIndex) {
            displayControl.showOptions(mode.options, mode.numOptions, focusIndex);
            lastRenderedFocusIndex = focusIndex;
        }

        if (encoderControl.checkMovementWithWrap(mode.numOptions)) {
            focusIndex = encoderControl.getCurrentIndex();
        }

        if (encoderControl.isShortPress()) {
            const int selectedOption = focusIndex;
            encoderControl.setCurrentIndex(savedMiniatureIndex);

            Serial0.print("Selected option: ");
            Serial0.println(mode.options[selectedOption]);

            // Minimal action wiring (we can expand this per-mode next)
            if (modeIndex == 0 && selectedOption == 0) {
                addNewMiniature();
            }

            optionSelected = true;
        }

        delay(10);
    }

    encoderControl.setCurrentIndex(savedMiniatureIndex);
}