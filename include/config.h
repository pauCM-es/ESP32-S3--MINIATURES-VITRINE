#ifndef CONFIG_H
#define CONFIG_H

// Pin definitions
// NeoPixel LED strip - Using GPIO48 (compatible with LED output)
#define LED_PIN 48
#define NUM_LEDS 3

// OLED Display (I2C)
#define OLED_SDA 8
#define OLED_SCL 9
#define OLED_ADDR 0x3C  // Common address for SSD1306/SSD1315 OLED displays (might be 0x3D)
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Rotary Encoder
// Using GPIO pins with interrupt capability
#define ENCODER_PIN_A 15  // GPIO15 (RTC/GPIO/TOUCH capable)
#define ENCODER_PIN_B 16  // GPIO16 (RTC/GPIO/TOUCH capable)
#define ENCODER_BUTTON 14 // GPIO14 (RTC/GPIO/TOUCH capable)

// Additional Button
#define MODE_BUTTON 17    // GPIO17 (RTC/GPIO/TOUCH capable)

// Animation and timing constants
#define POSITION_DISPLAY_TIME 2000  // ms to display a position
#define ANIMATION_SPEED 500         // ms between animation steps
#define PAUSE_TIME 1000             // ms pause between cycles

// Maximum number of miniatures (positions)
#define MAX_MINIATURES 3  // For now, just 3 positions

// Demo data structure for miniatures
struct Miniature {
    const char* name;
    const char* author;
    const char* date;
};

// Sample miniature data
const Miniature DEMO_MINIATURES[MAX_MINIATURES] = {
    {"Knight", "John Smith", "2020-01-15"},
    {"Dragon", "Maria Garcia", "2021-05-22"},
    {"Castle", "David Lee", "2022-11-03"}
};

#endif // CONFIG_H
