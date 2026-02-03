#include "SettingsMode.h"
#include "hardware/ModeManager.h"

namespace Modes {

void settings_backlightBrightness(ModeManager& manager) {
    static const char* const options[] = {
        "0%",
        "20%",
        "40%",
        "60%",
        "80%",
        "100%",
    };
    static const uint8_t values[] = {0, 20, 40, 60, 80, 100};
    static const int numOptions = sizeof(values) / sizeof(values[0]);

    int initial = 0;
    const uint8_t current = manager.getBacklightBrightnessPercent();
    for (int i = 0; i < numOptions; i++) {
        if (values[i] == current) {
            initial = i;
            break;
        }
    }

    manager.selectMode(options, numOptions, [&](int idx) {
        if (idx < 0 || idx >= numOptions) {
            return;
        }
        manager.setBacklightBrightnessPercent(values[idx]);

        char msg[48];
        snprintf(msg, sizeof(msg), "Backlight: %u%%", static_cast<unsigned>(values[idx]));
        manager.showStatus("Settings", msg);
        delay(600);
         }, initial, initial);
}

void settings_ledBrightness(ModeManager& manager) {
    static const char* const options[] = {
        "10%",
        "20%",
        "40%",
        "60%",
        "80%",
        "100%",
    };
    static const uint8_t values[] = {10, 20, 40, 60, 80, 100};
    static const int numOptions = sizeof(values) / sizeof(values[0]);

    int initial = 2;
    const uint8_t current = manager.getLedBrightnessPercent();
    for (int i = 0; i < numOptions; i++) {
        if (values[i] == current) {
            initial = i;
            break;
        }
    }

    manager.selectMode(options, numOptions, [&](int idx) {
        if (idx < 0 || idx >= numOptions) {
            return;
        }
        manager.setLedBrightnessPercent(values[idx]);
        char msg[48];
        snprintf(msg, sizeof(msg), "LED: %u%%", static_cast<unsigned>(values[idx]));
        manager.showStatus("Settings", msg);
        delay(600);
    }, initial, initial);
}

void settings_standbyBrightness(ModeManager& manager) {
    static const char* const options[] = {
        "5%",
        "10%",
        "20%",
        "30%",
        "40%",
    };
    static const uint8_t values[] = {5, 10, 20, 30, 40};
    static const int numOptions = sizeof(values) / sizeof(values[0]);

    int initial = 3;
    const uint8_t current = manager.getStandbyBrightnessPercent();
    for (int i = 0; i < numOptions; i++) {
        if (values[i] == current) {
            initial = i;
            break;
        }
    }

    manager.selectMode(options, numOptions, [&](int idx) {
        if (idx < 0 || idx >= numOptions) {
            return;
        }
        manager.setStandbyBrightnessPercent(values[idx]);
        char msg[48];
        snprintf(msg, sizeof(msg), "Standby: %u%%", static_cast<unsigned>(values[idx]));
        manager.showStatus("Settings", msg);
        delay(600);
    }, initial, initial);
}

void settings_ambientSpeed(ModeManager& manager) {
    static const char* const topOptions[] = {
        "Random speed",
        "Random brightness",
        "Random density",
        "All lights bright",
        "Back",
    };

    manager.selectMode(topOptions, 5, [&](int idx) {
        if (idx < 0 || idx >= 5) {
            return;
        }

        if (idx == 0) {
            static const char* const speedOptions[] = {"Slow", "Normal", "Fast"};
            static const uint16_t frameMs[] = {80, 40, 20};
            static const uint8_t step[] = {4, 6, 10};
            int initial = 1;
            const uint16_t current = manager.getAmbientRandomFrameMs();
            for (int i = 0; i < 3; i++) {
                if (frameMs[i] == current) {
                    initial = i;
                    break;
                }
            }
            manager.selectMode(speedOptions, 3, [&](int sIdx) {
                if (sIdx < 0 || sIdx >= 3) {
                    return;
                }
                manager.setAmbientRandomSpeed(frameMs[sIdx], step[sIdx]);
                char msg[48];
                snprintf(msg, sizeof(msg), "Random: %s", speedOptions[sIdx]);
                manager.showStatus("Settings", msg);
                delay(600);
            }, initial, initial);
            return;
        }

        if (idx == 1) {
            static const char* const brightOptions[] = {"20%", "40%", "60%", "80%"};
            static const uint8_t values[] = {20, 40, 60, 80};
            int initial = 1;
            const uint8_t current = manager.getAmbientRandomMaxBrightnessPercent();
            for (int i = 0; i < 4; i++) {
                if (values[i] == current) {
                    initial = i;
                    break;
                }
            }
            manager.selectMode(brightOptions, 4, [&](int bIdx) {
                if (bIdx < 0 || bIdx >= 4) {
                    return;
                }
                manager.setAmbientRandomMaxBrightnessPercent(values[bIdx]);
                char msg[48];
                snprintf(msg, sizeof(msg), "Random max: %u%%", static_cast<unsigned>(values[bIdx]));
                manager.showStatus("Settings", msg);
                delay(600);
            }, initial, initial);
            return;
        }

        if (idx == 2) {
            static const char* const densityOptions[] = {"2", "4", "6", "8"};
            static const uint8_t values[] = {2, 4, 6, 8};
            int initial = 2;
            const uint8_t current = manager.getAmbientRandomDensity();
            for (int i = 0; i < 4; i++) {
                if (values[i] == current) {
                    initial = i;
                    break;
                }
            }
            manager.selectMode(densityOptions, 4, [&](int dIdx) {
                if (dIdx < 0 || dIdx >= 4) {
                    return;
                }
                manager.setAmbientRandomDensity(values[dIdx]);
                char msg[48];
                snprintf(msg, sizeof(msg), "Random density: %u", static_cast<unsigned>(values[dIdx]));
                manager.showStatus("Settings", msg);
                delay(600);
            }, initial, initial);
            return;
        }

        if (idx == 3) {
            static const char* const allOptions[] = {"10%", "25%", "50%", "75%"};
            static const uint8_t values[] = {10, 25, 50, 75};
            int initial = 1;
            const uint8_t current = manager.getAmbientAllLightsBrightnessPercent();
            for (int i = 0; i < 4; i++) {
                if (values[i] == current) {
                    initial = i;
                    break;
                }
            }
            manager.selectMode(allOptions, 4, [&](int aIdx) {
                if (aIdx < 0 || aIdx >= 4) {
                    return;
                }
                manager.setAmbientAllLightsBrightnessPercent(values[aIdx]);
                char msg[48];
                snprintf(msg, sizeof(msg), "All lights: %u%%", static_cast<unsigned>(values[aIdx]));
                manager.showStatus("Settings", msg);
                delay(600);
            }, initial, initial);
            return;
        }

        // Back
    }, 4);
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
    }, initial, initial);
}

void settings_reset(ModeManager& manager) {
    const bool ok = manager.resetPersistedSettings();
    manager.showStatus("Settings", ok ? "Reset OK" : "Reset failed");
    delay(900);
}

} // namespace Modes
