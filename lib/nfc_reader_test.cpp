#include <ArduinoJson.h>
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

    // Create a JSON document to store the parsed data
    StaticJsonDocument<512> jsonDoc; // Increased size for larger JSON strings

    if (nfcReader.readTagUID(uid, uidLength)) {
        Serial0.print("NFC Tag UID: ");
        for (uint8_t i = 0; i < uidLength; i++) {
            Serial0.print(uid[i], HEX);
            if (i < uidLength - 1) {
                Serial0.print(":");
            }
        }
        Serial0.println();

        // Read and parse the tag context
        if (nfcReader.readTagContext(uid, uidLength, jsonDoc)) {
            // Access the parsed JSON fields
            const char* name = jsonDoc["name"];
            const char* team = jsonDoc["team"];
            const char* designBy = jsonDoc["designBy"];
            const char* painted = jsonDoc["painted"];

            // Debugging: Check each field
            if (name) {
                Serial0.print("Name: ");
                Serial0.println(name);
            } else {
                Serial0.println("Name field is missing.");
            }

            if (team) {
                Serial0.print("Team: ");
                Serial0.println(team);
            } else {
                Serial0.println("Team field is missing.");
            }

            if (designBy) {
                Serial0.print("Designed By: ");
                Serial0.println(designBy);
            } else {
                Serial0.println("Designed By field is missing.");
            }

            if (painted) {
                Serial0.print("Painted By: ");
                Serial0.println(painted);
            } else {
                Serial0.println("Painted By field is missing.");
            }
        } else {
            Serial0.println("Failed to read or parse tag context.");
        }
    } else {
        Serial0.println("Failed to read the NFC tag UID.");
    }

    delay(1000); // Wait 1 second before checking again
}