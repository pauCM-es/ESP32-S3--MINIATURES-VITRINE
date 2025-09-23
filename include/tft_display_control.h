#ifndef TFT_DISPLAY_CONTROL_H
#define TFT_DISPLAY_CONTROL_H

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>
#include "config.h"

class TFTDisplayControl {
private:
    Adafruit_ST7789* display;
    
    // Color definitions for 16-bit color
    static const uint16_t BLACK = 0x0000;
    static const uint16_t WHITE = 0xFFFF;
    static const uint16_t RED = 0xF800;
    static const uint16_t GREEN = 0x07E0;
    static const uint16_t BLUE = 0x001F;
    static const uint16_t YELLOW = 0xFFE0;
    static const uint16_t CYAN = 0x07FF;
    static const uint16_t MAGENTA = 0xF81F;
    static const uint16_t ORANGE = 0xF8C0;
    
public:
    // Constructor
    TFTDisplayControl();
    
    // Initialize display
    bool begin();
    
    // Clear display
    void clear();
    
    // Fill screen with color
    void fillScreen(uint16_t color);
    
    // Display miniature information
    void showMiniatureInfo(int index);
    void showInfo(const char* title, const char* subtitle, const char* author, const char* date);
    void showMode(const char* mode, const char* message);
    // Show a message at the specified location
    void showMessage(const char* message, int x, int y, int size = 1, uint16_t color = WHITE);
    void showWrappedMessage (const char* message, int x, int y, int size = 1, uint16_t color = WHITE);
    // Show a centered title
    void showTitle(const char* title, uint16_t color = WHITE);
    void showSubTitle(const char* title, uint16_t color = WHITE);
    
    void getCenterXPosition(const char* text, int& centerXPosition);
    // Utility method to get display pointer for advanced operations
    Adafruit_ST7789* getDisplay();
    
    // Color helpers
    uint16_t color565(uint8_t r, uint8_t g, uint8_t b);
    uint16_t getBlackColor() { return BLACK; }
    uint16_t getWhiteColor() { return WHITE; }
    uint16_t getRedColor() { return RED; }
    uint16_t getGreenColor() { return GREEN; }
    uint16_t getBlueColor() { return BLUE; }
    uint16_t getYellowColor() { return YELLOW; }
    uint16_t getCyanColor() { return CYAN; }
    uint16_t getMagentaColor() { return MAGENTA; }
    uint16_t getOrangeColor() { return ORANGE; }
};

#endif // TFT_DISPLAY_CONTROL_H