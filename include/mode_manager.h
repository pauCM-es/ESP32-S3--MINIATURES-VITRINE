#ifndef MODE_MANAGER_H
#define MODE_MANAGER_H

#include "led_movement_control.h"
#include "nfc_reader_control.h"
#include "tft_display_control.h"
#include "encoder_control.h"

struct Mode {
    const char* name; // Mode name
    const char* options[5]; // List of options for the mode
    int numOptions; // Number of options
};

const Mode MODES[] = {
    {"Add Mini", {"Start NFC Read"}, 1},
    {"Settings", {"Standby Light Brightness", "Speed ambient lights", "Reset"}, 3},
    {"Ambient Light", {"All Lights", "Random", "Rainbow"}, 3},
    {"Miniature Info", {"View Details"}, 1}
};
const int NUM_MODES = sizeof(MODES) / sizeof(MODES[0]);

class ModeManager {
public:
    ModeManager(LedMovementControl& ledMovementControl, NFCReaderControl& nfcReader, TFTDisplayControl& displayControl, EncoderControl& encoderControl);

    void addNewMiniature();
    void setStandbyBrightness(uint8_t brightness);
    void selectMode(const char* const options[], int numOptions, std::function<void(int)> callback);
    void handleModeOptions(int modeIndex);

private:
    LedMovementControl& ledMovementControl;
    NFCReaderControl& nfcReader;
    TFTDisplayControl& displayControl;
    EncoderControl& encoderControl;
};

#endif