#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#define LED_PIN 48
#define NUM_LEDS 1

Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  strip.begin();
  strip.show();
}

void loop() {
  strip.setPixelColor(0, strip.Color(255, 255, 255));
  strip.show();
  delay(500);

  strip.setPixelColor(0, strip.Color(0, 0, 0));
  strip.show();
  delay(500);
}
