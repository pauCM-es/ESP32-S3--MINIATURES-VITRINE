#include "SettingsStore.h"

#include <Preferences.h>

namespace {
constexpr const char* kNamespace = "vitrine";
constexpr uint16_t kVersion = 1;

// Keys
constexpr const char* kKeyVersion = "ver";
constexpr const char* kKeySleepTimeoutMinutes = "sleepMin";
constexpr const char* kKeyBacklightPercent = "blPct";
constexpr const char* kKeyLedPercent = "ledPct";
constexpr const char* kKeyStandbyPercent = "stbyPct";

constexpr const char* kKeyLastMiniIdx = "lastMini";
constexpr const char* kKeyLastModeIdx = "lastMode";

constexpr const char* kKeyWifiStaEnabled = "wStaEn";
constexpr const char* kKeyWifiStaSsid = "wStaS";
constexpr const char* kKeyWifiStaPass = "wStaP";
constexpr const char* kKeyWifiApSsid = "wApS";
constexpr const char* kKeyWifiApPass = "wApP";

constexpr const char* kKeyAmbientAllPct = "ambAll";
constexpr const char* kKeyAmbientRndMaxPct = "ambRMax";
constexpr const char* kKeyAmbientRndDensity = "ambRDen";
constexpr const char* kKeyAmbientRndFrameMs = "ambRFms";
constexpr const char* kKeyAmbientRndStep = "ambRStep";

uint8_t clampPercent(int value) {
    if (value < 0) return 0;
    if (value > 100) return 100;
    return static_cast<uint8_t>(value);
}

void loadString(Preferences& prefs, const char* key, char* outBuf, size_t outSize, const char* defaultValue) {
    if (!outBuf || outSize == 0) {
        return;
    }
    String v = prefs.getString(key, defaultValue ? defaultValue : "");
    strncpy(outBuf, v.c_str(), outSize - 1);
    outBuf[outSize - 1] = '\0';
}
}

bool SettingsStore::load(DeviceSettings& out) {
    Preferences prefs;
    if (!prefs.begin(kNamespace, true)) {
        return false;
    }

    const uint16_t ver = prefs.getUShort(kKeyVersion, 0);
    (void)ver; // reserved for future migrations

    out.sleepTimeoutMinutes = prefs.getUShort(kKeySleepTimeoutMinutes, out.sleepTimeoutMinutes);
    out.backlightBrightnessPercent = clampPercent(prefs.getUChar(kKeyBacklightPercent, out.backlightBrightnessPercent));

    out.ledBrightnessPercent = clampPercent(prefs.getUChar(kKeyLedPercent, out.ledBrightnessPercent));
    out.standbyBrightnessPercent = clampPercent(prefs.getUChar(kKeyStandbyPercent, out.standbyBrightnessPercent));

    out.lastMiniatureIndex = prefs.getUChar(kKeyLastMiniIdx, out.lastMiniatureIndex);
    out.lastMainModeIndex = static_cast<int8_t>(prefs.getChar(kKeyLastModeIdx, out.lastMainModeIndex));

    out.ambientAllLightsBrightnessPercent = clampPercent(prefs.getUChar(kKeyAmbientAllPct, out.ambientAllLightsBrightnessPercent));
    out.ambientRandomMaxBrightnessPercent = clampPercent(prefs.getUChar(kKeyAmbientRndMaxPct, out.ambientRandomMaxBrightnessPercent));

    const uint8_t density = prefs.getUChar(kKeyAmbientRndDensity, out.ambientRandomDensity);
    out.ambientRandomDensity = density == 0 ? out.ambientRandomDensity : density;

    const uint16_t frameMs = prefs.getUShort(kKeyAmbientRndFrameMs, out.ambientRandomFrameMs);
    out.ambientRandomFrameMs = frameMs == 0 ? out.ambientRandomFrameMs : frameMs;

    const uint8_t step = prefs.getUChar(kKeyAmbientRndStep, out.ambientRandomStep);
    out.ambientRandomStep = step == 0 ? out.ambientRandomStep : step;

    out.wifiStaEnabled = prefs.getUChar(kKeyWifiStaEnabled, out.wifiStaEnabled ? 1 : 0) != 0;
    loadString(prefs, kKeyWifiStaSsid, out.wifiStaSsid, sizeof(out.wifiStaSsid), out.wifiStaSsid);
    loadString(prefs, kKeyWifiStaPass, out.wifiStaPass, sizeof(out.wifiStaPass), out.wifiStaPass);
    loadString(prefs, kKeyWifiApSsid, out.wifiApSsid, sizeof(out.wifiApSsid), out.wifiApSsid);
    loadString(prefs, kKeyWifiApPass, out.wifiApPass, sizeof(out.wifiApPass), out.wifiApPass);

    prefs.end();
    return true;
}

bool SettingsStore::save(const DeviceSettings& settings) {
    Preferences prefs;
    if (!prefs.begin(kNamespace, false)) {
        return false;
    }

    prefs.putUShort(kKeyVersion, kVersion);
    prefs.putUShort(kKeySleepTimeoutMinutes, settings.sleepTimeoutMinutes);
    prefs.putUChar(kKeyBacklightPercent, clampPercent(settings.backlightBrightnessPercent));

    prefs.putUChar(kKeyLedPercent, clampPercent(settings.ledBrightnessPercent));
    prefs.putUChar(kKeyStandbyPercent, clampPercent(settings.standbyBrightnessPercent));

    prefs.putUChar(kKeyLastMiniIdx, settings.lastMiniatureIndex);
    prefs.putChar(kKeyLastModeIdx, static_cast<int8_t>(settings.lastMainModeIndex));

    prefs.putUChar(kKeyAmbientAllPct, clampPercent(settings.ambientAllLightsBrightnessPercent));
    prefs.putUChar(kKeyAmbientRndMaxPct, clampPercent(settings.ambientRandomMaxBrightnessPercent));
    prefs.putUChar(kKeyAmbientRndDensity, settings.ambientRandomDensity);
    prefs.putUShort(kKeyAmbientRndFrameMs, settings.ambientRandomFrameMs);
    prefs.putUChar(kKeyAmbientRndStep, settings.ambientRandomStep);

    prefs.putUChar(kKeyWifiStaEnabled, settings.wifiStaEnabled ? 1 : 0);
    prefs.putString(kKeyWifiStaSsid, settings.wifiStaSsid);
    prefs.putString(kKeyWifiStaPass, settings.wifiStaPass);
    prefs.putString(kKeyWifiApSsid, settings.wifiApSsid);
    prefs.putString(kKeyWifiApPass, settings.wifiApPass);

    prefs.end();
    return true;
}

bool SettingsStore::reset() {
    Preferences prefs;
    if (!prefs.begin(kNamespace, false)) {
        return false;
    }
    const bool ok = prefs.clear();
    prefs.end();
    return ok;
}
