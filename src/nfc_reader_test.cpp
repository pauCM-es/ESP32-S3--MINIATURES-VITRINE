#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PN532.h>
#include "nfc_reader_control.h"
#include "config.h"

// Create global instances
TwoWire customWire = TwoWire(1); // Use I2C bus 1
Adafruit_PN532 nfc(NFC_SDA, NFC_SCL, &customWire); // Use the I2C pins defined in config.h

// Create an instance of NFCReaderControl
NFCReaderControl nfcReader(&customWire, &nfc);

void setup() {
    // Initialize serial communication
    Serial0.begin(115200);
    Serial0.println("Starting NFC reader...");

    // customWire.begin(NFC_SDA, NFC_SCL); // Initialize the custom Wire instance
    // Initialize the NFC reader
    if (!nfcReader.begin()) {
        Serial0.println("Failed to initialize the NFC reader!");
        while (1); // Halt execution if initialization fails
    }

    Serial0.println("NFC reader initialized successfully.");
}

void loop() {
    uint8_t uid[7];
    uint8_t uidLength;

    if (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength)) {
        Serial0.print("NFC Tag UID: ");
        for (uint8_t i = 0; i < uidLength; i++) {
            Serial0.print(uid[i], HEX);
            if (i < uidLength - 1) {
                Serial0.print(":");
            }
        }
        Serial0.println();
    } else {
        Serial0.println("No NFC tag detected");
    }

    // if (nfcReader.isTagPresent()) {
    //     if (nfcReader.readTagUID(uid, uidLength)) {
    //         Serial0.print("NFC Tag UID: ");
    //         for (uint8_t i = 0; i < uidLength; i++) {
    //             Serial0.print(uid[i], HEX);
    //             if (i < uidLength - 1) {
    //                 Serial0.print(":");
    //             }
    //         }
    //         Serial0.println();
    //     } else {
    //         Serial0.println("Failed to read the NFC tag UID.");
    //     }
    // } else {
    //     Serial0.println("No NFC tag detected.");
    // }

    delay(1000); // Wait 1 second before checking again
}