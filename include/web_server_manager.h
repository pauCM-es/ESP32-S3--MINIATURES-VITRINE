#pragma once

#include <WebServer.h>
#include <LittleFS.h>

class WebServerManager {
public:
    WebServerManager();
    
    void begin();
    void handleClient();
    
    bool isFsMounted() const { return fsMounted; }

private:
    WebServer server;
    bool fsMounted;
    
    void setupRoutes();
    void handleApiInfo();
    void handleRoot();
    void handleIndexHtml();
    void handleNotFound();
    
    bool streamFileFromLittleFS(const String &path);
    static const char* getContentType(const String &path);
    static bool isReservedNonSpaPath(const String &path);
    static bool isStaticAssetPath(const String &path);
};
