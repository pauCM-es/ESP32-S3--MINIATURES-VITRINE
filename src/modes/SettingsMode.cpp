#include "SettingsMode.h"
#include "hardware/ModeManager.h"

namespace Modes {

void settings_backlightBrightness(ModeManager& manager) {
    manager.showStatus("Settings", "Backlight: TODO");
}

void settings_ambientSpeed(ModeManager& manager) {
    manager.showStatus("Settings", "Ambient speed: TODO");
}

void settings_sleepTimeout(ModeManager& manager) {
    static const char* const options[] = {
        "Off",
        "1 min",
        "2 min",
        "5 min",
        "10 min",
        "30 min",
    };
    static const uint16_t minutesForOption[] = {0, 1, 2, 5, 10, 30};
    static const int numOptions = sizeof(minutesForOption) / sizeof(minutesForOption[0]);

    // Pick initial focus based on current value
    int initial = 0;
    const uint16_t current = manager.getSleepTimeoutMinutes();
    for (int i = 0; i < numOptions; i++) {
        if (minutesForOption[i] == current) {
            initial = i;
            break;
        }
    }

    manager.selectMode(options, numOptions, [&](int idx) {
        if (idx < 0 || idx >= numOptions) {
            return;
        }
        manager.setSleepTimeoutMinutes(minutesForOption[idx]);

        char msg[48];
        if (minutesForOption[idx] == 0) {
            snprintf(msg, sizeof(msg), "Sleep: Off");
        } else {
            snprintf(msg, sizeof(msg), "Sleep: %u min", static_cast<unsigned>(minutesForOption[idx]));
        }
        manager.showStatus("Settings", msg);
        delay(600);
    }, initial);
}

void settings_reset(ModeManager& manager) {
    manager.showStatus("Settings", "Reset: TODO");
}

} // namespace Modes
