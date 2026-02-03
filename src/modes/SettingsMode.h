#pragma once

class ModeManager;

namespace Modes {

void settings_backlightBrightness(ModeManager& manager);
void settings_ambientSpeed(ModeManager& manager);
void settings_sleepTimeout(ModeManager& manager);
void settings_reset(ModeManager& manager);

} // namespace Modes
