#include "SettingsMode.h"
#include "hardware/ModeManager.h"

namespace Modes {

void settings_backlightBrightness(ModeManager& manager) {
    manager.showStatus("Settings", "Backlight: TODO");
}

void settings_ambientSpeed(ModeManager& manager) {
    manager.showStatus("Settings", "Ambient speed: TODO");
}

void settings_reset(ModeManager& manager) {
    manager.showStatus("Settings", "Reset: TODO");
}

} // namespace Modes
