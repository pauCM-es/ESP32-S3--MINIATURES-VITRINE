#ifndef CONFIG_H
#define CONFIG_H

// Pin definitions
// NeoPixel LED strip - Using GPIO48 (compatible with LED output)
#define LED_PIN 5
#define LED_INTERNAL_PIN 48
#define NUM_LEDS 6

// // OLED Display (I2C)
// #define OLED_SDA 8
// #define OLED_SCL 9
// #define OLED_ADDR 0x3C  // Common address for SSD1306/SSD1315 OLED displays (might be 0x3D)
// #define SCREEN_WIDTH 128
// #define SCREEN_HEIGHT 32

// TFT Display (ST7789 1.9inch IPS)
// Using hardware SPI pins for best performance
#define TFT_CS   10  // CS pin (Chip Select)
#define TFT_DC   14  // DC pin (Data/Command)
#define TFT_RST  18  // RES pin (Reset)
#define TFT_SCLK 12  // [SCL] Hardware SPI SCLK
#define TFT_MOSI 11  // [SDA] Hardware SPI MOSI
#define TFT_BLK  19  // BLK pin (Backlight control - PWM capable)
// Dimensions for 1.9" ST7789 display
#define TFT_WIDTH 240   // Common resolution for ST7789
#define TFT_HEIGHT 320  // Common resolution for ST7789

// Rotary Encoder
// Using GPIO pins with interrupt capability
#define ENCODER_PIN_A 15  // GPIO15 (RTC/GPIO/TOUCH capable)
#define ENCODER_PIN_B 16  // GPIO16 (RTC/GPIO/TOUCH capable)
#define ENCODER_BUTTON 17 // GPIO17

// Mode button
#define BTN_MODE 7    // GPIO7

// NFC Module (I2C)  
#define NFC_SDA 8  // SDA pin for NFC module
#define NFC_SCL 9  // SCL pin for NFC module
#define NFC_ADDR 0x24  // Default I2C address for PN532 (check your module's documentation)

// // Additional Button
// #define MODE_BUTTON 9    // GPIO9

// Animation and timing constants
#define POSITION_DISPLAY_TIME 2000  // ms to display a position
#define PAUSE_TIME 1000             // ms pause between cycles

// Maximum number of miniatures (positions)
#define MAX_MINIATURES 6  // For now, just 6 positions

// Demo data structure for miniatures
struct Miniature {
    const char* name;
    const char* author;
    const char* date;
};

// Sample miniature data
const Miniature DEMO_MINIATURES[MAX_MINIATURES] = {
    {"Captain America", "Marvel United", "Mayo 2025"},
    {"Batman", "Hall of Heroes", "Junio 2025"},
    {"Corrupted Dwarves", "StationForge", "Julio 2025"},
    {"Cyberpunk", "Heroes Infinite", "Agosto 2025"},
    {"Elf Archer", "Fantasy Miniatures", "Septiembre 2025"},
    {"Space Marine", "Galactic Warriors", "Octubre 2025"}
};

#endif // CONFIG_H
