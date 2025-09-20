#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "config.h"
#include "encoder_control.h"

// Crear instancia del controlador del encoder
EncoderControl encoderControl;

// Índice actual del encoder
int currentIndex = 0;

// Configuración del LED WS2812B
Adafruit_NeoPixel internalNeoPxLed(1, LED_INTERNAL_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel ledStrip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  // Inicializar comunicación serie
  Serial.begin(115200);
  Serial.println("Iniciando prueba del encoder y LED WS2812B...");

  // Inicializar el encoder
  encoderControl.begin();
  Serial.println("Encoder inicializado.");

  // Inicializar el botón en el pin 9
  pinMode(MODE_BUTTON, INPUT_PULLUP);
  Serial.println("Botón en el pin 9 inicializado.");

  // Inicializar el LED WS2812B
  internalNeoPxLed.begin();
  internalNeoPxLed.setBrightness(50); // Establecer brillo al 50%
  internalNeoPxLed.setPixelColor(0, internalNeoPxLed.Color(0, 255, 0)); // Establecer en verde
  internalNeoPxLed.show();

  ledStrip.begin();
  ledStrip.setBrightness(50); // Establecer brillo al 50%
  ledStrip.setPixelColor(0, ledStrip.Color(255, 0, 0)); // Establecer en verde
  ledStrip.show();
  Serial.println("LED WS2812B configurado en verde.");
}

void loop() {
  // Verificar movimiento del encoder
  if (encoderControl.checkMovement()) {
    // Actualizar el índice actual
    currentIndex = encoderControl.getCurrentIndex();
    Serial.print("Movimiento detectado. Posición actual: ");
    Serial.println(currentIndex);
  }

  // Verificar si el botón del encoder fue presionado
  if (encoderControl.isButtonPressed()) {
    Serial.println("¡Botón del encoder presionado!");
  }

  // Verificar si el botón en el pin 9 fue presionado
  if (digitalRead(MODE_BUTTON) == LOW) {
    Serial.println("¡Botón en el pin 9 presionado!");
    internalNeoPxLed.setPixelColor(0, internalNeoPxLed.Color(255, 255, 0)); // Cambiar a amarillo
    internalNeoPxLed.show();

    ledStrip.setPixelColor(0, ledStrip.Color(0, 0, 255)); // Cambiar a amarillo
    ledStrip.show();
    delay(200); // Retardo de rebote
  }

  // Pequeña pausa para evitar saturar la CPU
  delay(10);
}