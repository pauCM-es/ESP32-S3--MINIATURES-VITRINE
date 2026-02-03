#include "SettingsStore.h"

#include <Preferences.h>

namespace {
constexpr const char* kNamespace = "vitrine";
constexpr uint16_t kVersion = 1;

// Keys
constexpr const char* kKeyVersion = "ver";
constexpr const char* kKeySleepTimeoutMinutes = "sleepMin";
constexpr const char* kKeyBacklightPercent = "blPct";

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

    out.ambientAllLightsBrightnessPercent = clampPercent(prefs.getUChar(kKeyAmbientAllPct, out.ambientAllLightsBrightnessPercent));
    out.ambientRandomMaxBrightnessPercent = clampPercent(prefs.getUChar(kKeyAmbientRndMaxPct, out.ambientRandomMaxBrightnessPercent));

    const uint8_t density = prefs.getUChar(kKeyAmbientRndDensity, out.ambientRandomDensity);
    out.ambientRandomDensity = density == 0 ? out.ambientRandomDensity : density;

    const uint16_t frameMs = prefs.getUShort(kKeyAmbientRndFrameMs, out.ambientRandomFrameMs);
    out.ambientRandomFrameMs = frameMs == 0 ? out.ambientRandomFrameMs : frameMs;

    const uint8_t step = prefs.getUChar(kKeyAmbientRndStep, out.ambientRandomStep);
    out.ambientRandomStep = step == 0 ? out.ambientRandomStep : step;

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

    prefs.putUChar(kKeyAmbientAllPct, clampPercent(settings.ambientAllLightsBrightnessPercent));
    prefs.putUChar(kKeyAmbientRndMaxPct, clampPercent(settings.ambientRandomMaxBrightnessPercent));
    prefs.putUChar(kKeyAmbientRndDensity, settings.ambientRandomDensity);
    prefs.putUShort(kKeyAmbientRndFrameMs, settings.ambientRandomFrameMs);
    prefs.putUChar(kKeyAmbientRndStep, settings.ambientRandomStep);

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
