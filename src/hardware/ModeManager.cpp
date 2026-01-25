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

void ModeManager::selectMode(const char* const options[], int numOptions, std::function<void(int)> callback) {
    int focusIndex = 0;
    bool optionSelected = false;

    while (!optionSelected) {
        displayControl.showOptions(options, numOptions, focusIndex);

        if (encoderControl.checkMovement()) {
            focusIndex = encoderControl.getCurrentIndex() % numOptions;
        }

        if (encoderControl.isShortPress()) {
            callback(focusIndex);
            optionSelected = true;
        }

        delay(10);
    }
}

void ModeManager::handleModeOptions(int modeIndex) {
    const Mode& mode = MODES[modeIndex];
    int focusIndex = 0;
    bool optionSelected = false;

    while (!optionSelected) {
        displayControl.showOptions(mode.options, mode.numOptions, focusIndex);

        if (encoderControl.checkMovement()) {
            focusIndex = encoderControl.getCurrentIndex() % mode.numOptions;
        }

        if (encoderControl.isShortPress()) {
            Serial0.print("Selected option: ");
            Serial0.println(mode.options[focusIndex]);
            optionSelected = true;
        }

        delay(10);
    }
}