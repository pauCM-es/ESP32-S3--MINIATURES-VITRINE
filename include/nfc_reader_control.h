#ifndef NFC_READER_CONTROL_H
#define NFC_READER_CONTROL_H

#include <Arduino.h>
#include <Adafruit_PN532.h>

class NFCReaderControl {
private:
    Adafruit_PN532* nfc; // Pointer to the NFC reader object
    TwoWire* wire;       // Pointer to the custom Wire instance

public:
    // Constructor
    NFCReaderControl(TwoWire* wireInstance, Adafruit_PN532* nfcInstance);

    // Initialize the NFC reader
    bool begin();

    // Check if an NFC tag is present
    bool isTagPresent();

    // Read the UID of the NFC tag
    bool readTagUID(uint8_t* uidBuffer, uint8_t& uidLength);
};

#endif // NFC_READER_CONTROL_H