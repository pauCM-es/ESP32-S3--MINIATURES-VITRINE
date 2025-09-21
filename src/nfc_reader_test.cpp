#include <Arduino.h>
#include <Adafruit_PN532.h>
#include "nfc_reader_control.h"
#include "config.h"

// Create an instance of NFCReaderControl
NFCReaderControl nfcReader;

void setup() {
    // Initialize serial communication
    Serial0.begin(115200);
    Serial0.println("Starting NFC reader...");

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

        if (nfcReader.readTagUID(uid, uidLength)) {
            Serial0.print("NFC Tag UID: ");
            for (uint8_t i = 0; i < uidLength; i++) {
                Serial0.print(uid[i], HEX);
                if (i < uidLength - 1) {
                    Serial0.print(":");
                }
            }
            Serial0.println();
        } else {
            Serial0.println("Failed to read the NFC tag UID.");
        }

    delay(1000); // Wait 1 second before checking again
}