#include "wifi_manager.h"
#include "log.h"

#if __has_include("secrets.h")
#include "secrets.h"
#define HAS_WIFI_SECRETS 1
#else
#define HAS_WIFI_SECRETS 0
#endif

#ifndef AP_SSID
#define AP_SSID "Vitrine-ESP32S3"
#endif

#ifndef AP_PASS
#define AP_PASS "vitrine1234"
#endif

void WifiManager::begin() {
    WiFi.mode(WIFI_MODE_APSTA);
    
    if (HAS_WIFI_SECRETS) {
        connectStation();
    }
    
    startAccessPoint();
}

bool WifiManager::connectStation() {
#if HAS_WIFI_SECRETS
    LOGI("wifi", "Connecting to SSID: %s", WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    
    const uint32_t startMs = millis();
    while (WiFi.status() != WL_CONNECTED && (millis() - startMs) < 15000) {
        delay(250);
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        LOGI("wifi", "Connected, IP: %s", WiFi.localIP().toString().c_str());
        return true;
    } else {
        LOGW("wifi", "STA connect timeout; falling back to AP only");
        return false;
    }
#else
    return false;
#endif
}

void WifiManager::startAccessPoint() {
    const char *apSsid = AP_SSID;
    const char *apPass = AP_PASS;
    
    if (WiFi.softAP(apSsid, apPass)) {
        LOGI("wifi", "AP started: %s", apSsid);
        LOGI("wifi", "AP password: (hidden)");
        LOGI("wifi", "AP IP: %s", WiFi.softAPIP().toString().c_str());
    } else {
        LOGW("wifi", "Failed to start AP");
    }
}

const char* WifiManager::getActiveIP() const {
    static String ipStr;
    if (WiFi.status() == WL_CONNECTED) {
        ipStr = WiFi.localIP().toString();
    } else {
        ipStr = WiFi.softAPIP().toString();
    }
    return ipStr.c_str();
}

bool WifiManager::isStationConnected() const {
    return WiFi.status() == WL_CONNECTED;
}
