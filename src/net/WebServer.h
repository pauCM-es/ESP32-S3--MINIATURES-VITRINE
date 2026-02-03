#pragma once

#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include "WsServer.h"
#include "OtaFirmware.h"

class ModeManager;

class WebServer {
public:
    WebServer();
    
    void begin(ModeManager* modeManager = nullptr);
    
    bool isFsMounted() const { return fsMounted; }
    WsServer* getWsServer() { return &wsServer; }

private:
    AsyncWebServer server;
    WsServer wsServer;
    OtaFirmware otaFirmware;
    bool fsMounted;
    ModeManager* modeManager;
    
    void setupRoutes();
    void handleApiInfo(AsyncWebServerRequest *request);
    void handleApiFs(AsyncWebServerRequest *request);
    void handleApiSettingsGet(AsyncWebServerRequest *request);
    void handleApiSettingsPost(AsyncWebServerRequest *request, uint8_t* data, size_t len, size_t index, size_t total);
    void handleStaticFile(AsyncWebServerRequest *request);
    void handleNotFound(AsyncWebServerRequest *request);
    
    static const char* getContentType(const String &path);
    static bool isReservedNonSpaPath(const String &path);
    static bool isStaticAssetPath(const String &path);
};
