#include "AddMiniMode.h"
#include "hardware/ModeManager.h"

namespace Modes {

void addMini_startNfcRead(ModeManager& manager) {
    manager.addNewMiniature();
}

} // namespace Modes
