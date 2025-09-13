# Miniature Display Cabinet Project

This project implements a **smart display cabinet** for miniatures using an **ESP32-S3**. Each shelf contains individually addressable LEDs (WS2812B), an OLED screen for displaying information, and user inputs (rotary encoder, buttons). Future extensions include NFC reading, microSD storage, and wireless control.

---

## 📌 Hardware Overview

* **ESP32-S3** (main controller)
* **WS2812B LED strip** (matrix 10 × 7 → 70 LEDs)
* **OLED Display 0.96'' (128x64, SSD1315, I2C)**
* **Rotary Encoder with push button**
* **Extra push button** (mode switch)
* **NFC Module v3 (likely PN532)** → planned for later phase
* **MicroSD card adapter (SPI)** → planned for later phase

---

## 🚀 Development Phases

### Phase 1 – Core (Local navigation)

**Goal:** Show miniature info on the OLED and highlight the LED in focus.

* Connect the OLED via I2C.
* Connect the rotary encoder (CLK, DT, SW pins).
* Connect the WS2812B LED strip (single GPIO, typically with a level shifter).
* Write firmware that:

  * Keeps an internal index of the selected miniature (0–69).
  * Shows dummy data (name, author, date) on the OLED.
  * Lights only one LED corresponding to the selected index.
  * Rotating the encoder changes the selection.
  * Encoder push button may serve as a future confirmation.

### Phase 2 – NFC Integration

**Goal:** Read NTAG215 tags with the NFC module.

* Connect the NFC module (I2C or SPI depending on version).
* Read UID and data blocks from the tag.
* Show the tag data on the OLED.
* Indicate position status on LED strip (yellow = selecting, red = occupied, green = saved).

### Phase 3 – microSD Storage

**Goal:** Store and retrieve miniature data.

* Connect the microSD module (SPI: MISO, MOSI, SCK, CS).
* Implement CSV or JSON storage format.
* Load database from SD on startup.
* Save new/updated miniature info to SD.
* Validate read/write cycles.

### Phase 4 – Local Integration

**Goal:** Fully functional offline cabinet.

* On startup:

  * Load miniature database from SD.
  * Show the current miniature on the OLED.
  * Highlight the LED of the miniature.
* User can:

  * Navigate with encoder.
  * Add a miniature with NFC.
  * Save data to SD.
  * LEDs reflect occupied/free state.

---

## 📂 Recommended Libraries (Arduino framework)

* **OLED (SSD1315)** → `Adafruit_SSD1306`
* **Rotary Encoder** → `Encoder.h` or `AiEsp32RotaryEncoder`
* **WS2812B LEDs** → `Adafruit_NeoPixel` or `FastLED`
* **NFC (PN532)** → `Adafruit_PN532`
* **microSD** → `SD.h` (or `SdFat.h` for advanced usage)

---

## 🔧 Example Wiring (ESP32-S3)

> GPIOs can be remapped, these are suggested defaults.

* **OLED I2C** → SDA: GPIO 21, SCL: GPIO 22
* **LED Strip** → GPIO 5
* **Rotary Encoder** → CLK: GPIO 34, DT: GPIO 35, SW: GPIO 32
* **Extra Button** → GPIO 33
* **microSD (SPI)** → CS: GPIO 9, MOSI: GPIO 11, MISO: GPIO 12, SCK: GPIO 10

---

## 🛠️ Next Steps

1. Implement Phase 1 (OLED + encoder + LEDs).
2. Test navigation between 70 slots.
3. Display placeholder data for each slot.
4. Prepare for NFC integration (Phase 2).

---

## 📖 Notes

* All code, comments, and documentation must be written in **English**.
* The project will start **offline and local**. Wireless features (WiFi/Bluetooth) will be added in later phases.
