#ifndef SETTINGS_STORE_H
#define SETTINGS_STORE_H

#include <Arduino.h>
#include "DeviceSettings.h"

class SettingsStore {
public:
    static bool load(DeviceSettings& out);
    static bool save(const DeviceSettings& settings);
    static bool reset();
};

#endif
