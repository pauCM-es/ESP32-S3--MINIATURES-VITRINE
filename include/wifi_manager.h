#pragma once

#include <WiFi.h>

class WifiManager {
public:
    void begin();
    
    const char* getActiveIP() const;
    bool isStationConnected() const;

private:
    bool connectStation();
    void startAccessPoint();
};
