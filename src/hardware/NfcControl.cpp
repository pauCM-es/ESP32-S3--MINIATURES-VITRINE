#include "NfcControl.h"
#include "config.h"
#include <Wire.h>
#include <ArduinoJson.h> // Include the ArduinoJson library

// Constructor
NFCReaderControl::NFCReaderControl() {
  wire = new TwoWire(1); // Use I2C bus 1
  nfc = new Adafruit_PN532(NFC_SDA, NFC_SCL, wire);
}

// Initialize the NFC reader
bool NFCReaderControl::begin() {
    if (!wire->begin(NFC_SDA, NFC_SCL)) { // Initialize the custom Wire instance
        Serial0.println("[begin] Custom Wire initialization failed!");
        return false;
    }

    if (!nfc->begin()) {
        Serial0.println("[begin] NFC object is not initialized!");
        return false;
    }

    uint32_t versionData = nfc->getFirmwareVersion();
    if (!versionData) {
        Serial0.println("Didn't find PN53x board");
        return false;
    }

    // Configure the board to read NFC tags
    nfc->SAMConfig();
    Serial0.println("NFC reader initialized");
    return true;
}

// Read the UID of the NFC tag
bool NFCReaderControl::readTagUID(uint8_t* uidBuffer, uint8_t& uidLength) {
    if (nfc->readPassiveTargetID(PN532_MIFARE_ISO14443A, uidBuffer, &uidLength)) {
        return true;
    }
    return false;
}

bool NFCReaderControl::readTagContext(uint8_t* uidBuffer, uint8_t uidLength, JsonDocument& jsonDoc) {
    uint8_t data[32]; // Buffer to store the tag data
    String result = ""; // Accumulate decoded text

    // Read multiple pages
    for (uint8_t page = 4; page < 42; page++) {
        if (nfc->ntag2xx_ReadPage(page, data)) {
            // Process the data
            for (uint8_t i = 0; i < 4; i++) {
                if (data[i] > 0x1F && data[i] != 0xFE) { // Ignore non-printable characters
                    result += (char)data[i];
                }
            }
        } else {
            Serial0.println("[readTagContext] Failed to read page.");
            return false;
        }
    }

    // Extract the JSON object
    int jsonStart = result.indexOf('{'); // Find the start of the JSON object
    int jsonEnd = result.lastIndexOf('}'); // Find the end of the JSON object

    if (jsonStart != -1 && jsonEnd != -1 && jsonEnd > jsonStart) {
        String jsonObject = result.substring(jsonStart, jsonEnd + 1); // Extract the JSON object
        Serial0.print("[readTagContext] Extracted JSON: ");
        Serial0.println(jsonObject);

        // Parse the JSON object
        DeserializationError error = deserializeJson(jsonDoc, jsonObject);
        if (error) {
            Serial0.print("[readTagContext] JSON parsing failed: ");
            Serial0.println(error.c_str());
            return false;
        }

        // Successfully parsed JSON
        Serial0.println("[readTagContext] JSON parsed successfully.");
        return true;
    } else {
        Serial0.println("[readTagContext] No valid JSON object found.");
        return false;
    }
}