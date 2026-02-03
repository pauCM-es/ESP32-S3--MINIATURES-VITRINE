#include "AmbientLightMode.h"
#include "hardware/ModeManager.h"

namespace Modes {

void ambient_allLights(ModeManager& manager) {
    manager.ambientAllLights();
}

void ambient_random(ModeManager& manager) {
    manager.ambientRandom();
}

} // namespace Modes
