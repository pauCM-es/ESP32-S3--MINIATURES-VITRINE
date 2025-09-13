#ifndef DISPLAY_CONTROL_H
#define DISPLAY_CONTROL_H

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "config.h"

class DisplayControl {
private:
    Adafruit_SSD1306* display;
    
public:
    // Constructor
    DisplayControl();
    
    // Initialize display
    bool begin();
    
    // Clear display
    void clear();
    
    // Display miniature information
    void showMiniatureInfo(int index);
    
    // Show a message at the specified location
    void showMessage(const char* message, int x, int y, int size = 1);
    
    // Show a centered title
    void showTitle(const char* title);
    
    // Show navigation help
    void showNavHelp();
};

#endif // DISPLAY_CONTROL_H
