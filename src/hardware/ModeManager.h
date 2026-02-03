#ifndef MODE_MANAGER_H
#define MODE_MANAGER_H

#include "LedMovementControl.h"
#include "NfcControl.h"
#include "DisplayControl.h"
#include "EncoderControl.h"

#include <functional>

class ModeManager {
public:
    ModeManager(LedMovementControl& ledMovementControl, NFCReaderControl& nfcReader, TFTDisplayControl& displayControl, EncoderControl& encoderControl);

    void addNewMiniature();
    void setStandbyBrightness(uint8_t brightness);

    // Small UI helper for mode actions implemented outside ModeManager
    void showStatus(const char* title, const char* message);

    // Ambient helpers
    void ambientAllLights();
    void ambientRandom();

    // Sleep helpers
    void enterSleep();
    void wakeFromSleep(int currentIndex);
    bool isSleeping() const;
    void setSleepTimeoutMinutes(uint16_t minutes);
    uint16_t getSleepTimeoutMinutes() const;
    uint32_t getSleepTimeoutMs() const;
    bool isSleepMode(int modeIndex) const;

    // Show the top-level list of modes (via ModesRegistry) and return the selected mode index
    void selectMainMode(std::function<void(int)> callback);

    void selectMode(const char* const options[], int numOptions, std::function<void(int)> callback, int initialFocusIndex = 0);
    void handleModeOptions(int modeIndex);

    // Introspection helpers for callers (e.g., logging)
    int getNumModes() const;
    const char* getModeName(int modeIndex) const;

private:
    LedMovementControl& ledMovementControl;
    NFCReaderControl& nfcReader;
    TFTDisplayControl& displayControl;
    EncoderControl& encoderControl;

    bool sleeping = false;
    uint16_t sleepTimeoutMinutes = 5;
};

#endif