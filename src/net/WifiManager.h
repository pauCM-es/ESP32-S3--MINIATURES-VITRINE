#pragma once

#include <WiFi.h>

#include "util/DeviceSettings.h"

class WifiManager {
public:
    void begin();
    void begin(const DeviceSettings& settings);
    
    const char* getActiveIP() const;
    bool isStationConnected() const;

private:
    bool connectStation(const DeviceSettings* settings);
    void startAccessPoint(const DeviceSettings* settings);
};
