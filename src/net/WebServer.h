#pragma once

#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include "WsServer.h"

class WebServer {
public:
    WebServer();
    
    void begin();
    
    bool isFsMounted() const { return fsMounted; }
    WsServer* getWsServer() { return &wsServer; }

private:
    AsyncWebServer server;
    WsServer wsServer;
    bool fsMounted;
    
    void setupRoutes();
    void handleApiInfo(AsyncWebServerRequest *request);
    void handleStaticFile(AsyncWebServerRequest *request);
    void handleNotFound(AsyncWebServerRequest *request);
    
    static const char* getContentType(const String &path);
    static bool isReservedNonSpaPath(const String &path);
    static bool isStaticAssetPath(const String &path);
};
