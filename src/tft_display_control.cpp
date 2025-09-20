#include "tft_display_control.h"

// Constructor
TFTDisplayControl::TFTDisplayControl() {
    // Initialize the ST7789 display with CS, DC, and RST pins
    display = new Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
}

// Initialize display
bool TFTDisplayControl::begin() {
    // Configure backlight pin (if available)
    #ifdef TFT_BLK
    pinMode(TFT_BLK, OUTPUT);
    digitalWrite(TFT_BLK, HIGH); // Turn on backlight
    #endif
    
    // Initialize SPI using hardware SPI pins
    // VSPI on ESP32-S3: SCLK=GPIO12, MISO=GPIO13, MOSI=GPIO11
    SPI.begin(TFT_SCLK, -1, TFT_MOSI, -1); // SCLK, MISO, MOSI, SS (we manage CS manually)
    
    // Initialize the display with specific width and height for the 1.9" ST7789
    display->init(TFT_WIDTH, TFT_HEIGHT);
    
    // Set rotation (0-3) - may need to be adjusted based on your specific display orientation
    display->setRotation(1);
    
    // Fill with black to clear any initial artifacts
    display->fillScreen(BLACK);
    
    // Display initial message to confirm the display is working
    display->setTextSize(2);
    display->setTextColor(WHITE);
    display->setCursor(10, 10);
    display->println("ST7789 TFT Display");
    display->setTextSize(1);
    display->setCursor(10, 40);
    display->println("1.9 inch IPS Display");
    display->setCursor(10, 60);
    display->println("ESP32-S3 Miniatures Vitrine");
    display->setCursor(10, 80);
    display->println("Initializing...");
    
    // Draw a border
    drawBorder(CYAN);
    
    // Delay to show the startup message
    delay(2000);
    
    return true;
}

// Clear display
void TFTDisplayControl::clear() {
    display->fillScreen(BLACK);
}

// Fill screen with color
void TFTDisplayControl::fillScreen(uint16_t color) {
    display->fillScreen(color);
}

// Display miniature information
void TFTDisplayControl::showMiniatureInfo(int index) {
    if (index < 0 || index >= MAX_MINIATURES) {
        return;
    }
    
    clear();
    drawBorder(BLUE);
    
    // Display title
    display->setTextSize(3);
    display->setTextColor(YELLOW);
    display->setCursor(10, 20);
    display->println(DEMO_MINIATURES[index].name);
    
    // Display position
    display->setTextSize(1);
    display->setTextColor(WHITE);
    display->setCursor(10, 60);
    display->print("Position: ");
    display->println(index + 1);  // Display 1-based position for user
    
    // Display author
    display->setTextSize(2);
    display->setCursor(10, 80);
    display->print("Design by: ");
    display->setTextColor(CYAN);
    display->println(DEMO_MINIATURES[index].author);
    
    // Display date
    display->setTextColor(WHITE);
    display->setCursor(10, 120);
    display->print("Date: ");
    display->setTextColor(GREEN);
    display->println(DEMO_MINIATURES[index].date);
}

// Show a message at the specified location
void TFTDisplayControl::showMessage(const char* message, int x, int y, int size, uint16_t color) {
    display->setTextSize(size);
    display->setTextColor(color);
    display->setCursor(x, y);
    display->println(message);
}

// Show a centered title
void TFTDisplayControl::showTitle(const char* title, uint16_t color) {
    int16_t x1, y1;
    uint16_t w, h;
    
    display->setTextSize(2);
    display->setTextColor(color);
    
    // Calculate width of title text
    display->getTextBounds(title, 0, 0, &x1, &y1, &w, &h);
    
    // Center the text
    display->setCursor((TFT_WIDTH - w) / 2, 10);
    display->println(title);
}

// Show navigation help
void TFTDisplayControl::showNavHelp() {
    display->setTextSize(1);
    display->setTextColor(YELLOW);
    display->setCursor(10, TFT_HEIGHT - 20);
    display->println("< Turn encoder to navigate >");
}

// Draw a simple border
void TFTDisplayControl::drawBorder(uint16_t color) {
    display->drawRect(0, 0, TFT_HEIGHT, TFT_WIDTH, color);
    display->drawRect(2, 2, TFT_HEIGHT - 4, TFT_WIDTH - 4, color);
}

// Utility method to get display pointer for advanced operations
Adafruit_ST7789* TFTDisplayControl::getDisplay() {
    return display;
}

// Convert RGB values to 16-bit color
uint16_t TFTDisplayControl::color565(uint8_t r, uint8_t g, uint8_t b) {
    return display->color565(r, g, b);
}