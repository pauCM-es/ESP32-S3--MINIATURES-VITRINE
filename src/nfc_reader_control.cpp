#include "nfc_reader_control.h"
#include "config.h"
#include <Wire.h>

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