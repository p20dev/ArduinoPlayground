#include <ArduinoGraphics.h>
#include <Arduino_LED_Matrix.h>

// Scrolling text demo for the Arduino UNO R4 WiFi 12x8 LED matrix.

constexpr unsigned long SCROLL_SPEED_MS = 75;
constexpr unsigned long REPEAT_PAUSE_MS = 1000;

const char MESSAGE[] =
  "Kan AI skrive programmer til Arduino? Tjek https://iot.p20.dev";

ArduinoLEDMatrix matrix;

void setup() {
  matrix.begin();
  matrix.textFont(Font_5x7);
  matrix.textScrollSpeed(SCROLL_SPEED_MS);
  matrix.clear();
}

void loop() {
  matrix.beginDraw();
  matrix.stroke(0xFFFFFFFF);
  matrix.beginText(12, 1, 0xFFFFFF);
  matrix.print(MESSAGE);
  matrix.endText(SCROLL_LEFT);
  matrix.endDraw();

  matrix.clear();
  delay(REPEAT_PAUSE_MS);
}

