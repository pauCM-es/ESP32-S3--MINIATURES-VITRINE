#include "MaintenanceMode.h"
#include "../util/Log.h"
#include "config.h"
#include <Arduino.h>

MaintenanceMode& MaintenanceMode::getInstance() {
    static MaintenanceMode instance;
    return instance;
}

MaintenanceMode::MaintenanceMode() : active(false) {}

void MaintenanceMode::enter() {
    if (!active) {
        active = true;
        LOGW("maint", "Entering maintenance mode");
    }
}

void MaintenanceMode::exit() {
    if (active) {
        active = false;
        LOGI("maint", "Exiting maintenance mode");
    }
}

bool MaintenanceMode::isActive() const {
    return active;
}

bool MaintenanceMode::checkBootTrigger() {
    // Check if BTN_MODE is held LOW at boot (future OTA trigger)
    pinMode(BTN_MODE, INPUT_PULLUP);
    delay(10); // Small debounce
    
    bool triggered = (digitalRead(BTN_MODE) == LOW);
    
    if (triggered) {
        LOGI("maint", "Maintenance mode triggered by button at boot");
    }
    
    return triggered;
}
