# Miniature Display Cabinet Project

This project implements a **smart display cabinet** for miniatur### Current Wiring

* **LED Strip** → GPIO 47 (changed from GPIO 48 to avoid onboard LED interference)
* **Display Options:**
  * **OLED I2C** → SDA: GPIO 8, SCL: GPIO 9
  * **TFT SPI** → CS: GPIO 10, DC: GPIO 14, RST: GPIO 18, SCK: GPIO 12 (HW), MOSI: GPIO 11 (HW), BLK: GPIO 19
* **Rotary Encoder** → CLK: GPIO 15, DT: GPIO 16, SW: GPIO 17 (cambiado de GPIO 14)
* **Extra Button** → GPIO 9 (cambiado de GPIO 17)
* **microSD (SPI)** → (Planned for later phase)g an **ESP32-S3**. Each shelf contains individually addressable LEDs (WS2812B), an OLED screen for displaying information, and user inputs (rotary encoder, buttons). Future extensions include NFC reading, microSD storage, and wireless control.

---

## 📌 Hardware Overview

* **ESP32-S3** (main controller)
* **WS2812B LED strip** (matrix 10 × 7 → 70 LEDs)
* **Display options:**
  * **OLED Display 0.96'' (128x64, SSD1315, I2C)** - Original option
  * **TFT Display 1.9" IPS (320x170, ST7789, SPI)** - Alternative option with color and higher resolution
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

## 📂 Project Structure

The project is organized into multiple files for better maintainability:

* **include/config.h** - Configuration constants and pin definitions
* **include/led_control.h** - LED strip control interface
* **include/display_control.h** - OLED display interface
* **include/encoder_control.h** - Rotary encoder interface
* **src/led_control.cpp** - LED strip control implementation
* **src/display_control.cpp** - OLED display implementation
* **src/encoder_control.cpp** - Rotary encoder implementation
* **src/main.cpp** - Main application logic integrating all components

## 📚 Used Libraries (Arduino framework)

* **Display Options:**
  * **OLED (SSD1306/SSD1315)** → `Adafruit_SSD1306`, `Adafruit_GFX`
  * **TFT (ST7789)** → `Adafruit_ST7789`, `Adafruit_GFX`
* **Rotary Encoder** → `ESP32Encoder`
* **WS2812B LEDs** → `Adafruit_NeoPixel`
* **NFC (PN532)** → `Adafruit_PN532` (planned for Phase 2)
* **microSD** → `SD.h` (planned for Phase 3)

---

## 🔧 ESP32-S3 Pin Definitions

![ESP32-S3 Pin Definitions](images/esp32s3_pin_definitions.jpg)

*Note: Save the image shown above to the `images` folder as `esp32s3_pin_definitions.jpg`*

### Current Wiring

* **OLED I2C** → SDA: GPIO 8, SCL: GPIO 9
* **LED Strip** → GPIO 48
* **Rotary Encoder** → CLK: GPIO 15, DT: GPIO 16, SW: GPIO 14
* **Extra Button** → GPIO 17
* **microSD (SPI)** → (Planned for later phase)

### Pin Selection Criteria

* **GPIO 47** for LED strip: Selected to avoid interference with the onboard RGB LED on GPIO 48.
* **GPIO 8 & 9** for OLED I2C: These pins support I2C functionality, making them suitable for the display.
* **GPIO 11 & 12** for TFT SPI: Using hardware SPI pins for maximum performance.
* **GPIO 15 & 16** for rotary encoder: These pins support interrupts which help with accurate encoder reading.
* **GPIO 17** for encoder button: Provides clean digital input with internal pull-up resistor.
* **GPIO 9** for mode button: Offers stable digital input for user interaction.

### GPIO Pin Types Glossary

| Category | Color | Description |
|----------|-------|-------------|
| **GPIO** | Green | General Purpose Input/Output - Standard digital pins for basic digital I/O operations |
| **ADC** | Purple | Analog-to-Digital Converter - Pins that can read analog voltage levels |
| **TOUCH** | Orange | Touch Sensor Input Channel - Pins that can detect capacitive touch |
| **RTC** | Blue | Real-Time Clock domain - Pins that remain powered during deep sleep |
| **JTAG/USB** | Dark Blue | JTAG debugging interface and USB functionality pins |
| **SERIAL** | Grey | Serial communication pins - Used for UART/serial debug and programming |
| **STRAP** | Magenta | Boot mode selection pins - Used during ESP32 startup |
| **SPI/MISC** | Light Green | SPI bus and miscellaneous functionality pins |
| **CLK** | Yellow | Clock output pins |

### Special Functions

| Abbreviation | Description |
|--------------|-------------|
| **MTDI, MTDO, MTMS, MTCK** | JTAG interface pins (Test Data In, Test Data Out, Test Mode Select, Test Clock) |
| **U0TXD, U0RXD** | UART0 Transmit and Receive pins (primary serial interface) |
| **SD_** | SD card interface signals |
| **SPICS**, **SPICLK**, **SPID**, **SPIQ** | SPI bus signals (Chip Select, Clock, Data, Queue) |
| **USB_D+**, **USB_D-** | USB data signals |
| **XTAL** | External crystal oscillator pins |
| **BOOT** | Boot mode selection pin |
| **VBUS** | USB bus voltage |
| **VDD33** | 3.3V power supply |
| **GND** | Ground reference |

### "Other" Special Tags (Yellowish Labels)

| Abbreviation | Description |
|--------------|-------------|
| **FSPICS0** | Flash SPI Chip Select - Used for external flash memory interface |
| **FSPICLK** | Flash SPI Clock - Clock signal for SPI flash |
| **FSPID** | Flash SPI Data - Data line for SPI flash |
| **FSPIQ** | Flash SPI Queue - SPI flash data line |
| **FSPIWP** | Flash SPI Write Protect - Controls write protection for SPI flash |
| **FSPIHD** | Flash SPI Hold - Hold signal for SPI flash |
| **XTAL_32K_P/N** | 32KHz crystal oscillator pins - Used for precise timing in low-power modes |
| **GPIO** | General Purpose Input/Output pins with specific numbering |
| **CLK_OUT1/2** | Clock output pins - Used to output internal clocks for external components |
| **SUBSPICS/HD/WP** | Secondary/Subsidiary SPI interface signals |
| **RGB LED** | Built-in addressable RGB LED control signals (on some ESP32-S3 dev boards) |

### Understanding Multi-Function Pins

Many pins on the ESP32-S3 have multiple functions which can be selected in software:

* A single pin might show multiple labels in the image (e.g., "GPIO14 / ADC2_3 / TOUCH14")
* When choosing pins for your project, consider all functions to avoid conflicts
* Some functions are mutually exclusive (you can only use one at a time)
* Key priority considerations:
  * Hardware-dedicated functions take precedence (USB, JTAG, etc.)
  * Boot mode pins have special requirements during startup
  * ADC pins should be chosen for analog sensors
  * Touch pins should be used for capacitive touch interfaces
  * I2C, SPI, and other communication protocols have preferred pins for optimal performance

---

## 🛠️ Current Progress

### Phase 1 - Initial Implementation

* ✅ Basic LED control with 3 positions implemented
* ✅ Different colors for each position
* ✅ Simple animation sequence added
* ✅ Serial monitoring for debugging

### Phase 1-2 - OLED Display and Encoder Integration

* ✅ Project refactored into multiple files for better maintainability
* ✅ Added OLED display integration (SSD1306/SSD1315)
* ✅ Added rotary encoder support with button functionality
* ✅ Implemented miniature information display
* ✅ Created sample data structure for miniature information

### Next Steps

1. Expand to full 70 LED positions
2. Implement NFC module integration (Phase 2)
3. Add microSD card storage (Phase 3)

---

## 📖 Notes

* The project will start **offline and local**. Wireless features (WiFi/Bluetooth) will be added in later phases.
