#include "display_control.h"

// Constructor
DisplayControl::DisplayControl() {
    display = new Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
}

// Initialize display
bool DisplayControl::begin() {
    Wire.begin(OLED_SDA, OLED_SCL);
    
    // Initialize display with address
    if(!display->begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
        Serial.println(F("SSD1306 allocation failed"));
        return false;
    }
    
    display->clearDisplay();
    display->setTextColor(SSD1306_WHITE);
    
    // Display startup screen
    display->clearDisplay();
    display->setTextSize(1);
    display->setCursor(0, 0);
    display->println(F("Miniatures Vitrine"));
    display->setCursor(0, 16);
    display->println(F("ESP32-S3 Project"));
    display->setCursor(0, 40);
    display->println(F("Initializing..."));
    display->display();
    delay(2000);
    
    return true;
}

// Clear display
void DisplayControl::clear() {
    display->clearDisplay();
    display->display();
}

// Display miniature information
void DisplayControl::showMiniatureInfo(int index) {
    if (index < 0 || index >= MAX_MINIATURES) {
        return;
    }
    
    display->clearDisplay();
    
    // Show position number
    display->setTextSize(1);
    display->setCursor(0, 0);
    display->print(F("Position: "));
    display->println(index);
    
    // Show miniature name
    display->setTextSize(2);
    display->setCursor(0, 16);
    display->println(DEMO_MINIATURES[index].name);
    
    // Show author
    display->setTextSize(1);
    display->setCursor(0, 36);
    display->print(F("By: "));
    display->println(DEMO_MINIATURES[index].author);
    
    // Show date
    display->setCursor(0, 48);
    display->print(F("Date: "));
    display->println(DEMO_MINIATURES[index].date);
    
    // Show navigation hint
    display->setCursor(0, 56);
    display->println(F("Turn encoder to navigate"));
    
    display->display();
}

// Show a message at the specified location
void DisplayControl::showMessage(const char* message, int x, int y, int size) {
    display->setTextSize(size);
    display->setCursor(x, y);
    display->println(message);
    display->display();
}

// Show a centered title
void DisplayControl::showTitle(const char* title) {
    display->setTextSize(1);
    int16_t x1, y1;
    uint16_t w, h;
    display->getTextBounds(title, 0, 0, &x1, &y1, &w, &h);
    display->setCursor((SCREEN_WIDTH - w) / 2, 0);
    display->println(title);
    display->display();
}

// Show navigation help
void DisplayControl::showNavHelp() {
    display->setTextSize(1);
    display->setCursor(0, 56);
    display->println(F("< Turn to navigate >"));
    display->display();
}
