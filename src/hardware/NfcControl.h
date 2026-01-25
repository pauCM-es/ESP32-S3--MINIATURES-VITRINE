#ifndef NFC_READER_CONTROL_H
#define NFC_READER_CONTROL_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <Adafruit_PN532.h>
#include <Wire.h>

class NFCReaderControl {
private:
    Adafruit_PN532* nfc; // Pointer to the NFC reader object
    TwoWire* wire;       // Pointer to the custom Wire instance

public:
    // Constructor
    NFCReaderControl();

    // Initialize the NFC reader
    bool begin();

    // Read the UID of the NFC tag
    bool readTagUID(uint8_t* uidBuffer, uint8_t& uidLength);

    bool readTagContext(uint8_t* uidBuffer, uint8_t uidLength, JsonDocument& jsonDoc);

};

#endif // NFC_READER_CONTROL_H