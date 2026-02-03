#ifndef MODE_MANAGER_H
#define MODE_MANAGER_H

#include "LedMovementControl.h"
#include "NfcControl.h"
#include "DisplayControl.h"
#include "EncoderControl.h"

#include "util/DeviceSettings.h"

#include <functional>

class ModeManager {
public:
    ModeManager(LedMovementControl& ledMovementControl, NFCReaderControl& nfcReader, TFTDisplayControl& displayControl, EncoderControl& encoderControl);

    // Load persisted settings (or use provided settings) and apply them to hardware
    void begin(const DeviceSettings* initialSettings = nullptr);

    // Call frequently from loop() to flush deferred persistence
    void tick();

    uint8_t getLastMiniatureIndex() const;
    void setLastMiniatureIndex(uint8_t index);

    void addNewMiniature();
    void setStandbyBrightness(uint8_t brightness);

    // Small UI helper for mode actions implemented outside ModeManager
    void showStatus(const char* title, const char* message);

    // Ambient helpers
    void ambientAllLights();
    void ambientRandom();

    // Persisted settings (used by Settings mode)
    void setBacklightBrightnessPercent(uint8_t percent);
    uint8_t getBacklightBrightnessPercent() const;

    void setLedBrightnessPercent(uint8_t percent);
    uint8_t getLedBrightnessPercent() const;

    void setStandbyBrightnessPercent(uint8_t percent);
    uint8_t getStandbyBrightnessPercent() const;

    void setAmbientAllLightsBrightnessPercent(uint8_t percent);
    uint8_t getAmbientAllLightsBrightnessPercent() const;

    void setAmbientRandomMaxBrightnessPercent(uint8_t percent);
    uint8_t getAmbientRandomMaxBrightnessPercent() const;

    void setAmbientRandomDensity(uint8_t density);
    uint8_t getAmbientRandomDensity() const;

    void setAmbientRandomSpeed(uint16_t frameMs, uint8_t step);
    uint16_t getAmbientRandomFrameMs() const;
    uint8_t getAmbientRandomStep() const;

    bool resetPersistedSettings();

    // WiFi persisted settings (applied on boot by WifiManager)
    bool getWifiStaEnabled() const;
    const char* getWifiStaSsid() const;
    bool hasWifiStaPass() const;
    const char* getWifiApSsid() const;
    bool hasWifiApPass() const;
    // Returns true if anything changed (and was persisted)
    bool setWifiStaConfig(bool enabled, const char* ssid, const char* pass, bool passProvided);
    // Returns true if anything changed (and was persisted)
    bool setWifiApConfig(const char* ssid, const char* pass, bool passProvided);

    // Sleep helpers
    void enterSleep();
    void wakeFromSleep(int currentIndex);
    bool isSleeping() const;
    void setSleepTimeoutMinutes(uint16_t minutes);
    uint16_t getSleepTimeoutMinutes() const;
    uint32_t getSleepTimeoutMs() const;
    bool isSleepMode(int modeIndex) const;

    // Show the top-level list of modes (via ModesRegistry) and return the selected mode index
    void selectMainMode(std::function<void(int)> callback);

    void selectMode(const char* const options[], int numOptions, std::function<void(int)> callback, int initialFocusIndex = 0, int selectedIndex = -1, const char* footerHint = nullptr);
    void handleModeOptions(int modeIndex);

    // Introspection helpers for callers (e.g., logging)
    int getNumModes() const;
    const char* getModeName(int modeIndex) const;

private:
    LedMovementControl& ledMovementControl;
    NFCReaderControl& nfcReader;
    TFTDisplayControl& displayControl;
    EncoderControl& encoderControl;

    bool sleeping = false;

    DeviceSettings settings;

    void applySettingsToHardware();
    bool persistSettings();

    // Deferred persistence for frequently updated values
    bool pendingSave = false;
    unsigned long pendingSaveDueMs = 0;
};

#endif