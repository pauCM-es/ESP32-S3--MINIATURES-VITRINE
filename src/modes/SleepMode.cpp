#include "SleepMode.h"
#include "hardware/ModeManager.h"

namespace Modes {

void sleep_enter(ModeManager& manager) {
    manager.showStatus("Sleep", "Zzz...");
    delay(150);
    manager.enterSleep();
}

} // namespace Modes
