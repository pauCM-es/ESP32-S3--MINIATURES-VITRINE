#include "ModesRegistry.h"

#include "AddMiniMode.h"
#include "AmbientLightMode.h"
#include "MiniatureInfoMode.h"
#include "SettingsMode.h"
#include "SleepMode.h"

namespace Modes {

static const ModeDef MODE_DEFS[] = {
    {
        "Add Mini",
        {"Start NFC Read"},
        1,
        {addMini_startNfcRead}
    },
    {
        "Settings",
        {"Backlight Brightness", "LED Brightness", "Standby Brightness", "Speed ambient lights", "Sleep timeout", "Power off", "Reset"},
        7,
        {settings_backlightBrightness, settings_ledBrightness, settings_standbyBrightness, settings_ambientSpeed, settings_sleepTimeout, settings_powerOff, settings_reset}
    },
    {
        "Sleep",
        {"Enter sleep"},
        1,
        {sleep_enter}
    },
    {
        "Ambient Light",
        {"All Lights", "Random"},
        2,
        {ambient_allLights, ambient_random}
    },
    {
        "Miniature Info",
        {"View Details"},
        1,
        {info_viewDetails}
    },
};

static const int MODE_COUNT = sizeof(MODE_DEFS) / sizeof(MODE_DEFS[0]);

int getNumModes() {
    return MODE_COUNT;
}

const ModeDef& getMode(int index) {
    if (index < 0) {
        return MODE_DEFS[0];
    }
    if (index >= MODE_COUNT) {
        return MODE_DEFS[MODE_COUNT - 1];
    }
    return MODE_DEFS[index];
}

} // namespace Modes
