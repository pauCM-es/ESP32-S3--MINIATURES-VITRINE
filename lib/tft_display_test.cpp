#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include "config.h"

/*
 * Script básico para probar la pantalla TFT ST7789
 * 
 * Conexiones:
 * - TFT CS  -> GPIO10
 * - TFT DC  -> GPIO14
 * - TFT RST -> GPIO18
 * - TFT SCK -> GPIO12 (Hardware SPI)
 * - TFT MOSI-> GPIO11 (Hardware SPI)
 * - TFT BLK -> GPIO19
 * 
 * Este script simplemente inicializa la pantalla y muestra texto
 * básico para verificar su funcionamiento.
 */

// Definir objeto de pantalla ST7789
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

// Colores para usar en la pantalla
#define BLACK    0x0000
#define WHITE    0xFFFF
#define RED      0xF800
#define GREEN    0x07E0
#define BLUE     0x001F
#define CYAN     0x07FF
#define MAGENTA  0xF81F
#define YELLOW   0xFFE0

void setup() {
  // Inicializar comunicación serial
  Serial.begin(9600);
  Serial.println("Test básico de pantalla TFT ST7789");

  // Configurar pin de backlight (si está disponible)
  pinMode(TFT_BLK, OUTPUT);
  digitalWrite(TFT_BLK, HIGH); // Encender retroiluminación
  Serial.println("Backlight encendido");
  
  // Inicializar SPI usando pines de hardware SPI
  SPI.begin(TFT_SCLK, -1, TFT_MOSI, -1); // SCLK, MISO, MOSI, SS (CS se maneja manualmente)
  Serial.println("SPI inicializado");
  
  // Inicializar la pantalla
  Serial.println("Inicializando pantalla TFT...");
  tft.init(TFT_WIDTH, TFT_HEIGHT);
  Serial.println("Pantalla inicializada");
  
  // Configurar rotación (0-3)
  tft.setRotation(1); // Configurar orientación horizontal
  
  // Limpiar la pantalla con color negro
  tft.fillScreen(BLACK);
  Serial.println("Pantalla limpiada");
  
  // Dibujar un borde para visualizar los límites de la pantalla
  tft.drawRect(0, 0,  TFT_HEIGHT, TFT_WIDTH, CYAN);
  tft.drawRect(2, 2, TFT_HEIGHT - 4, TFT_WIDTH - 4, CYAN);
  Serial.println("Borde dibujado");

  // Mostrar un título
  tft.setTextSize(2);
  tft.setTextColor(WHITE);
  tft.setCursor(10, 10);
  tft.println("TFT ST7789 Display Test");
  Serial.println("Título mostrado");
  
  // Mostrar información de configuración
  tft.setTextSize(1);
  tft.setTextColor(GREEN);
  tft.setCursor(10, 40);
  tft.print("Display Size: ");
  tft.print(TFT_WIDTH);
  tft.print("x");
  tft.println(TFT_HEIGHT);
  
  tft.setCursor(10, 60);
  tft.print("CS: GPIO");
  tft.print(TFT_CS);
  tft.print(", DC: GPIO");
  tft.println(TFT_DC);
  
  tft.setCursor(10, 80);
  tft.print("RST: GPIO");
  tft.print(TFT_RST);
  tft.print(", BLK: GPIO");
  tft.println(TFT_BLK);
  
  tft.setCursor(10, 100);
  tft.print("SCK: GPIO");
  tft.print(TFT_SCLK);
  tft.print(", MOSI: GPIO");
  tft.println(TFT_MOSI);
  
  // Mostrar una línea colorida
  tft.setTextSize(2);
  tft.setCursor(10, 130);
  tft.setTextColor(RED);
  tft.print("R");
  tft.setTextColor(GREEN);
  tft.print("G");
  tft.setTextColor(BLUE);
  tft.print("B");
  tft.setTextColor(YELLOW);
  tft.print("Y");
  tft.setTextColor(CYAN);
  tft.print("C");
  tft.setTextColor(MAGENTA);
  tft.print("M");
  
  Serial.println("Prueba de pantalla TFT completada.");
  Serial.println("Si puedes ver texto y colores en la pantalla, la prueba fue exitosa!");
}

void loop() {
  // Parpadear un punto en la esquina para mostrar que el programa sigue ejecutándose
  static bool dotState = false;
  dotState = !dotState;
  
  tft.fillRect(TFT_WIDTH - 10, TFT_HEIGHT - 10, 5, 5, dotState ? GREEN : BLACK);
  
  delay(500); // Parpadeo cada medio segundo
}