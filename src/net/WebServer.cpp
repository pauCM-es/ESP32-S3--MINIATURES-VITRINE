#include "WebServer.h"
#include "../util/Log.h"
#include "MaintenanceMode.h"
#include "version.h"
#include <ArduinoJson.h>
#include <WiFi.h>

WebServer::WebServer() : server(80), fsMounted(false) {}

void WebServer::begin() {
    // Mount LittleFS
    fsMounted = LittleFS.begin(true);
    if (!fsMounted) {
        LOGE("fs", "LittleFS mount failed");
    } else {
        LOGI("fs", "LittleFS mounted");
    }
    
    // Initialize WebSocket server
    wsServer.begin(&server);
    
    setupRoutes();
    server.begin();
    LOGI("http", "HTTP server started on port 80");
}

void WebServer::setupRoutes() {
    // API endpoints
    server.on("/api/info", HTTP_GET, [this](AsyncWebServerRequest *request) {
        handleApiInfo(request);
    });

    // Step 8: OTA firmware update
    otaFirmware.attach(server, wsServer);

    // Step 8: OTA upload page (served from LittleFS)
    server.on("/update", HTTP_GET, [this](AsyncWebServerRequest *request) {
    if (fsMounted && LittleFS.exists("/update/index.html")) {
        request->send(LittleFS, "/update/index.html", "text/html");
        return;
    }
    request->send(404, "text/plain", "Update page missing in LittleFS (/update/index.html)");
    });
    server.serveStatic("/update/", LittleFS, "/update/").setDefaultFile("index.html");
    
    // Static file handler (SPA fallback included)
    server.onNotFound([this](AsyncWebServerRequest *request) {
        handleNotFound(request);
    });
}

void WebServer::handleApiInfo(AsyncWebServerRequest *request) {
    JsonDocument doc;

    doc["firmwareVersion"] = FIRMWARE_VERSION;
    doc["webVersion"] = WEB_VERSION;

    char buildDate[32];
    snprintf(buildDate, sizeof(buildDate), "%s %s", __DATE__, __TIME__);
    doc["buildDate"] = buildDate;

    doc["uptime"] = static_cast<uint32_t>(millis() / 1000);

    String ip;
    if (WiFi.status() == WL_CONNECTED) {
        ip = WiFi.localIP().toString();
    } else {
        ip = WiFi.softAPIP().toString();
    }
    doc["ip"] = ip;

    doc["maintenanceMode"] = MaintenanceMode::getInstance().isActive();

    if (fsMounted) {
        const size_t total = LittleFS.totalBytes();
        const size_t used = LittleFS.usedBytes();
        doc["littlefsFree"] = (total >= used) ? (total - used) : 0;
    } else {
        doc["littlefsFree"] = 0;
    }

    String out;
    serializeJson(doc, out);
    
    AsyncWebServerResponse *response = request->beginResponse(200, "application/json", out);
    request->send(response);
}
void WebServer::handleNotFound(AsyncWebServerRequest *request) {
    const String path = request->url();

    // Step 4: SPA fallback routing
    // - Reserved paths (API/WS/OTA) never fall back
    // - Existing static files are served normally
    // - Non-static GET requests fall back to index.html
    if (isReservedNonSpaPath(path)) {
        request->send(404, "text/plain", "Not found");
        return;
    }

    // Try to serve the requested file
    if (fsMounted && LittleFS.exists(path)) {
        request->send(LittleFS, path, getContentType(path));
        return;
    }

    // If it looks like a static asset, return 404
    if (isStaticAssetPath(path)) {
        request->send(404, "text/plain", "Not found");
        return;
    }

    // SPA fallback: serve index.html for non-static GET requests
    if (request->method() == HTTP_GET && fsMounted && LittleFS.exists("/index.html")) {
        request->send(LittleFS, "/index.html", "text/html");
        return;
    }

    request->send(500, "text/plain", fsMounted ? "index.html missing in LittleFS" : "LittleFS not mounted");
}

const char* WebServer::getContentType(const String &path) {
    if (path.endsWith(".html")) return "text/html";
    if (path.endsWith(".css")) return "text/css";
    if (path.endsWith(".js")) return "application/javascript";
    if (path.endsWith(".json")) return "application/json";
    if (path.endsWith(".png")) return "image/png";
    if (path.endsWith(".jpg") || path.endsWith(".jpeg")) return "image/jpeg";
    if (path.endsWith(".svg")) return "image/svg+xml";
    if (path.endsWith(".ico")) return "image/x-icon";
    return "text/plain";
}

bool WebServer::isReservedNonSpaPath(const String &path) {
    if (path == "/ws") return true;
    if (path.startsWith("/api/")) return true;
    if (path == "/update") return true;
    if (path.startsWith("/update/")) return true;
    return false;
}

bool WebServer::isStaticAssetPath(const String &path) {
    // Treat common asset routes or any URL with a file extension as "static".
    if (path.startsWith("/assets/")) return true;
    const int lastSlash = path.lastIndexOf('/');
    const int lastDot = path.lastIndexOf('.');
    return lastDot > lastSlash;
}
