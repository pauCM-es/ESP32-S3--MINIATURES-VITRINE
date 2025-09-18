// #include <Arduino.h>
// #include <Wire.h>
// #include <Adafruit_GFX.h>
// #include <Adafruit_SSD1306.h>

// // Pin definitions for OLED Display (I2C)
// #define OLED_SDA 8
// #define OLED_SCL 9
// #define SCREEN_WIDTH 128
// #define SCREEN_HEIGHT 64

// // Display instance
// Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// void setup() {
//   // Initialize serial for debugging
//   Serial.begin(115200);
//   delay(500);
  
//   Serial.println("Test Básico de Pantalla OLED");
  
//   // Initialize I2C with specified pins
//   Wire.begin(OLED_SDA, OLED_SCL);
  
//   Serial.println("Intentando inicializar la pantalla...");
  
//   // Try common addresses
//   if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
//     if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3D)) {
//       Serial.println("No se pudo inicializar la pantalla OLED");
//       while (1) delay(100);
//     }
//   }
  
//   Serial.println("Pantalla inicializada correctamente");
  
//   // Clear the display
//   display.clearDisplay();
  
//   // Set text properties
//   display.setTextColor(SSD1306_WHITE);
  
//   // Display simple text
//   display.setTextSize(2);
//   display.setCursor(0, 0);
//   display.println("Test OLED");
  
//   // Actually show the content on the screen
//   display.display();
  
//   Serial.println("Texto mostrado en pantalla");
// }

// void loop() {
//   delay(100);
// }