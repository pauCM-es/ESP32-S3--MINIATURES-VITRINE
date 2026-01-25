#pragma once

#include <ESPAsyncWebServer.h>

class WsServer;

class OtaFirmware {
public:
    void attach(AsyncWebServer& server, WsServer& wsServer);
};
