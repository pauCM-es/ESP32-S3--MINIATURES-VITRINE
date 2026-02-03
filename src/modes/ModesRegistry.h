#pragma once

class ModeManager;

namespace Modes {

static constexpr int MAX_MODE_OPTIONS = 8;
static constexpr int MAX_MODES = 8;

using ModeAction = void (*)(ModeManager&);

struct ModeDef {
    const char* name;
    const char* options[MAX_MODE_OPTIONS];
    int numOptions;
    ModeAction actions[MAX_MODE_OPTIONS];
};

int getNumModes();
const ModeDef& getMode(int index);

} // namespace Modes
