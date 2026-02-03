#include "WifiManager.h"
#include "../util/Log.h"

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
    DeviceSettings defaults;
    begin(defaults);
}

void WifiManager::begin(const DeviceSettings& settings) {
    const bool staEnabled = settings.wifiStaEnabled;
    WiFi.mode(staEnabled ? WIFI_MODE_APSTA : WIFI_MODE_AP);

    if (staEnabled) {
        connectStation(&settings);
    }

    startAccessPoint(&settings);
}

static bool hasNonEmpty(const char* s) {
    return s && s[0] != '\0';
}

bool WifiManager::connectStation(const DeviceSettings* settings) {
    const char* ssid = nullptr;
    const char* pass = nullptr;

    if (settings && hasNonEmpty(settings->wifiStaSsid)) {
        ssid = settings->wifiStaSsid;
        pass = settings->wifiStaPass;
    }

#if HAS_WIFI_SECRETS
    if (!ssid || ssid[0] == '\0') {
        ssid = WIFI_SSID;
        pass = WIFI_PASS;
    }
#endif

    if (!ssid || ssid[0] == '\0') {
        LOGW("wifi", "STA disabled/no credentials; AP only");
        return false;
    }

    LOGI("wifi", "Connecting to SSID: %s", ssid);
    WiFi.begin(ssid, pass ? pass : "");
    
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
}

void WifiManager::startAccessPoint(const DeviceSettings* settings) {
    const char* apSsid = AP_SSID;
    const char* apPass = AP_PASS;

    if (settings && hasNonEmpty(settings->wifiApSsid)) {
        apSsid = settings->wifiApSsid;
        apPass = settings->wifiApPass;
    }
    
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
