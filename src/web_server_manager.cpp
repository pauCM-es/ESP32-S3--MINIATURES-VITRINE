#include "web_server_manager.h"
#include "log.h"
#include "version.h"
#include <ArduinoJson.h>
#include <WiFi.h>

WebServerManager::WebServerManager() : server(80), fsMounted(false) {}

void WebServerManager::begin() {
    // Mount LittleFS
    fsMounted = LittleFS.begin(true);
    if (!fsMounted) {
        LOGE("fs", "LittleFS mount failed");
    } else {
        LOGI("fs", "LittleFS mounted");
    }
    
    setupRoutes();
    server.begin();
    LOGI("http", "HTTP server started on port 80");
}

void WebServerManager::handleClient() {
    server.handleClient();
}

void WebServerManager::setupRoutes() {
    // API endpoints
    server.on("/api/info", HTTP_GET, [this]() { handleApiInfo(); });
    
    // Static routes
    server.on("/", HTTP_GET, [this]() { handleRoot(); });
    server.on("/index.html", HTTP_GET, [this]() { handleIndexHtml(); });
    
    // SPA fallback
    server.onNotFound([this]() { handleNotFound(); });
}

void WebServerManager::handleApiInfo() {
    StaticJsonDocument<384> doc;

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

    if (fsMounted) {
        const size_t total = LittleFS.totalBytes();
        const size_t used = LittleFS.usedBytes();
        doc["littlefsFree"] = (total >= used) ? (total - used) : 0;
    } else {
        doc["littlefsFree"] = 0;
    }

    String out;
    serializeJson(doc, out);
    server.send(200, "application/json", out);
}

void WebServerManager::handleRoot() {
    if (fsMounted && streamFileFromLittleFS("/index.html")) {
        return;
    }
    server.send(500, "text/plain", fsMounted ? "index.html missing in LittleFS" : "LittleFS not mounted");
}

void WebServerManager::handleIndexHtml() {
    if (fsMounted && streamFileFromLittleFS("/index.html")) {
        return;
    }
    server.send(404, "text/plain", fsMounted ? "Not found" : "LittleFS not mounted");
}

void WebServerManager::handleNotFound() {
    const String path = server.uri();

    // Step 4: SPA fallback routing
    // - Reserved paths (API/WS/OTA) never fall back
    // - Existing static files are served normally
    // - Non-static GET requests fall back to index.html
    if (isReservedNonSpaPath(path)) {
        server.send(404, "text/plain", "Not found");
        return;
    }

    if (fsMounted && streamFileFromLittleFS(path)) {
        return;
    }

    if (isStaticAssetPath(path)) {
        server.send(404, "text/plain", "Not found");
        return;
    }

    if (server.method() == HTTP_GET && fsMounted && streamFileFromLittleFS("/index.html")) {
        return;
    }

    server.send(500, "text/plain", fsMounted ? "index.html missing in LittleFS" : "LittleFS not mounted");
}

bool WebServerManager::streamFileFromLittleFS(const String &path) {
    if (!fsMounted) {
        return false;
    }
    if (!LittleFS.exists(path)) {
        return false;
    }
    File file = LittleFS.open(path, "r");
    if (!file) {
        return false;
    }
    server.streamFile(file, getContentType(path));
    file.close();
    return true;
}

const char* WebServerManager::getContentType(const String &path) {
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

bool WebServerManager::isReservedNonSpaPath(const String &path) {
    if (path == "/ws") return true;
    if (path.startsWith("/api/")) return true;
    if (path.startsWith("/update/")) return true;
    return false;
}

bool WebServerManager::isStaticAssetPath(const String &path) {
    // Treat common asset routes or any URL with a file extension as "static".
    if (path.startsWith("/assets/")) return true;
    const int lastSlash = path.lastIndexOf('/');
    const int lastDot = path.lastIndexOf('.');
    return lastDot > lastSlash;
}
