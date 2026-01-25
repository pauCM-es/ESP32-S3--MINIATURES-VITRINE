#include "DisplayControl.h"

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
    display->setRotation(3);
    
    // Fill with black to clear any initial artifacts
    display->fillScreen(BLACK);
    
    // Display initial message to confirm the display is working
    showMessage("Initializing...", 10, 10, 2, WHITE);
    
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

void TFTDisplayControl::showWrappedMessage (const char* message, int x, int y, int size, uint16_t color) {
    display->setTextSize(size);
    display->setTextColor(color);

    int cursorY = y; // Keep the initial Y position
    int maxWidth = TFT_WIDTH - x; // Maximum allowed width for the text
    char buffer[256]; // Buffer to handle the text
    strncpy(buffer, message, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0'; // Ensure the text is null-terminated

    char* token = strtok(buffer, " "); // Split the text into words
    String line = "";

    while (token != nullptr) {
        String word = String(token);
        int16_t x1, y1;
        uint16_t w, h;

        // Calculate the width of the current line with the new word
        display->getTextBounds((line + " " + word).c_str(), x, cursorY, &x1, &y1, &w, &h);

        if (w > maxWidth) {
            // If the line is too long, print the current line and start a new one
            display->setCursor(x, cursorY);
            display->println(line.c_str());
            cursorY += h; // Move the cursor to the next line
            line = word; // Start a new line with the current word
        } else {
            // If the line fits, add the word to the current line
            if (!line.isEmpty()) {
                line += " ";
            }
            line += word;
        }

        token = strtok(nullptr, " "); // Move to the next word
    }

    // Print the last line
    if (!line.isEmpty()) {
        display->setCursor(x, cursorY);
        display->println(line.c_str());
    }
}

// Display miniature information
void TFTDisplayControl::showMiniatureInfo(int index) {
    if (index < 0 || index >= MAX_MINIATURES) {
        return;
    }
    
    clear();

    // Fallback to "empty" if the demo entry is not initialized
    const char* name = DEMO_MINIATURES[index].name ? DEMO_MINIATURES[index].name : "empty";
    const char* author = DEMO_MINIATURES[index].author ? DEMO_MINIATURES[index].author : "empty";
    const char* date = DEMO_MINIATURES[index].date ? DEMO_MINIATURES[index].date : "empty";

    showTitle(name, YELLOW);
    showSubTitle(author, MAGENTA);

    showMessage(
        "Design by: ", 10, 80, 2, WHITE
    );
    showMessage(
        author, 10, 100, 2, CYAN
    );
    showMessage(
        "Painted: ", 10, 125, 2, WHITE
    );
    showMessage(
        date, 10, 145, 2, CYAN
    );
    
}

void TFTDisplayControl::showInfo(const char* title, const char* subtitle, const char* author, const char* date) {
    clear();
    
    showTitle(title, YELLOW);
    showSubTitle(subtitle, MAGENTA);

    showMessage(
        "Design by: ", 10, 80, 2, WHITE
    );
    showMessage(
        author, 10, 100, 2, CYAN
    );
    showMessage(
        "Painted: ", 10, 125, 2, WHITE
    );
    showMessage(
        date, 10, 145, 2, CYAN
    );
    
}

void TFTDisplayControl::showMode(const char* mode, const char* message) {
    clear();
    
    showTitle(mode, MAGENTA);
    int centerX = 0;
    getCenterXPosition(message, centerX);
    showMessage(
        message, centerX, 100, 2, CYAN
    );
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
    
    display->setTextSize(3);
    display->setTextColor(color);
    
    // Calculate width of title text
    display->getTextBounds(title, 0, 0, &x1, &y1, &w, &h);
    
    // Center the text
    display->setCursor((TFT_HEIGHT - w) / 2, 15);
    display->println(title);
}

void TFTDisplayControl::showSubTitle(const char* title, uint16_t color) {
    int16_t x1, y1;
    uint16_t w, h;

    display->setTextSize(2);
    display->setTextColor(color);
    
    // Calculate width of title text
    display->getTextBounds(title, 0, 0, &x1, &y1, &w, &h);
    
    // Center the text
    display->setCursor((TFT_HEIGHT - w) / 2, 45);
    display->println(title);
}

void TFTDisplayControl::getCenterXPosition(const char* text, int& centerXPosition) {
    int16_t x1, y1;
    uint16_t w, h;

    // Calculate width of text
    display->getTextBounds(text, 0, 0, &x1, &y1, &w, &h);

    centerXPosition = (TFT_HEIGHT - w) / 2;
}

// Utility method to get display pointer for advanced operations
Adafruit_ST7789* TFTDisplayControl::getDisplay() {
    return display;
}

// Convert RGB values to 16-bit color
uint16_t TFTDisplayControl::color565(uint8_t r, uint8_t g, uint8_t b) {
    return display->color565(r, g, b);
}

void TFTDisplayControl::showOptions(const char* const options[], int numOptions, int focusIndex) {
    clear();

    for (int i = 0; i < numOptions; i++) {
        int yPosition = 30 + i * 20; // Adjust spacing between options
        if (i == focusIndex) {
            // Highlight the focused option
            showMessage(options[i], 10, yPosition, 2, YELLOW);
        } else {
            // Regular option
            showMessage(options[i], 10, yPosition, 2, WHITE);
        }
    }
}