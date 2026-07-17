#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "config.h"

// Minimal strip diagnostics for WS2812B (GRB, 800kHz)
Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

void showSingleColor(uint8_t r, uint8_t g, uint8_t b, const char* label) {
  strip.clear();
  for (int i = 0; i < NUM_LEDS; i++) {
    strip.setPixelColor(i, strip.Color(r, g, b));
  }
  strip.show();
  Serial.printf("[TEST] %s\n", label);
}

void setup() {
  Serial.begin(115200);
  delay(500);

  Serial.println("=== WS2812B STRIP DIAGNOSTIC START ===");
  Serial.printf("Pin: %d | LEDs: %d\n", LED_PIN, NUM_LEDS);

  strip.begin();
  strip.clear();
  strip.setBrightness(64); // ~25% to keep current moderate
  strip.show();

  // Step 1: first LED only (checks data direction and first pixel health)
  strip.setPixelColor(0, strip.Color(255, 0, 0));
  strip.show();
  Serial.println("[TEST] Step 1: LED 0 should be RED");
  delay(2000);

  // Step 2: all LEDs red/green/blue
  showSingleColor(255, 0, 0, "Step 2: all RED");
  delay(1200);
  showSingleColor(0, 255, 0, "Step 2: all GREEN");
  delay(1200);
  showSingleColor(0, 0, 255, "Step 2: all BLUE");
  delay(1200);

  // Step 3: all white (RGB white)
  showSingleColor(128, 128, 128, "Step 3: all WHITE (RGB)");
  delay(1500);

  Serial.println("[TEST] Entering moving pixel loop");
}

void loop() {
  static int pos = 0;

  strip.clear();
  strip.setPixelColor(pos, strip.Color(120, 120, 120));
  strip.show();

  pos = (pos + 1) % NUM_LEDS;
  delay(80);
}
