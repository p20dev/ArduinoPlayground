#include <Arduino_LED_Matrix.h>
#include <WiFiS3.h>

#include "arduino_secrets.h"

constexpr uint8_t ROWS = 8;
constexpr uint8_t COLUMNS = 12;
constexpr unsigned long CONNECT_TIMEOUT_MS = 15000;
constexpr unsigned long RETRY_DELAY_MS = 3000;
constexpr unsigned long STATUS_REFRESH_MS = 500;

ArduinoLEDMatrix matrix;
uint8_t frame[ROWS][COLUMNS] = {};

void clearFrame() {
  memset(frame, 0, sizeof(frame));
}

void setPixel(int x, int y, bool on = true) {
  if (x >= 0 && x < COLUMNS && y >= 0 && y < ROWS) {
    frame[y][x] = on ? 1 : 0;
  }
}

void showFrame() {
  matrix.renderBitmap(frame, ROWS, COLUMNS);
}

void showConnecting(uint8_t step) {
  static const int8_t spinner[][2] = {
    {5, 0}, {7, 1}, {9, 2}, {10, 4}, {9, 6}, {7, 7},
    {5, 7}, {3, 6}, {1, 5}, {1, 3}, {2, 1}, {4, 0}
  };
  constexpr uint8_t pointCount = sizeof(spinner) / sizeof(spinner[0]);

  clearFrame();
  for (uint8_t trail = 0; trail < 4; ++trail) {
    const uint8_t index = (step + pointCount - trail) % pointCount;
    setPixel(spinner[index][0], spinner[index][1]);
  }
  showFrame();
}

void showConnected(long rssi, bool heartbeat) {
  clearFrame();

  // Check mark on the left.
  setPixel(0, 4);
  setPixel(1, 5);
  setPixel(2, 6);
  setPixel(3, 5);
  setPixel(4, 4);
  setPixel(5, 3);
  setPixel(6, 2);

  // Signal-strength bars on the right.
  const uint8_t bars = rssi >= -55 ? 3 : (rssi >= -70 ? 2 : 1);
  const uint8_t heights[] = {2, 4, 6};
  const uint8_t xPositions[] = {7, 9, 11};
  for (uint8_t bar = 0; bar < bars; ++bar) {
    for (uint8_t height = 0; height < heights[bar]; ++height) {
      setPixel(xPositions[bar], 7 - height);
    }
  }

  // A slow heartbeat shows that the sketch is still running.
  setPixel(0, 0, heartbeat);
  showFrame();
}

void showFailure() {
  clearFrame();
  for (uint8_t i = 1; i < 7; ++i) {
    setPixel(i + 2, i);
    setPixel(10 - i, i);
  }
  showFrame();
}

bool connectToHotspot() {
  Serial.print(F("Forbinder til hotspot: "));
  Serial.println(SECRET_SSID);

  WiFi.begin(SECRET_SSID, SECRET_PASS);
  const unsigned long startedAt = millis();
  uint8_t animationStep = 0;

  while (WiFi.status() != WL_CONNECTED &&
         millis() - startedAt < CONNECT_TIMEOUT_MS) {
    showConnecting(animationStep++);
    delay(120);
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println(F("Forbindelsen mislykkedes; proever igen om lidt."));
    showFailure();
    WiFi.disconnect();
    delay(RETRY_DELAY_MS);
    return false;
  }

  Serial.println(F("Wi-Fi forbundet."));
  Serial.print(F("IP-adresse: "));
  Serial.println(WiFi.localIP());
  Serial.print(F("Signalstyrke: "));
  Serial.print(WiFi.RSSI());
  Serial.println(F(" dBm"));

  showConnected(WiFi.RSSI(), true);
  delay(1200);
  return true;
}

void setup() {
  Serial.begin(115200);
  matrix.begin();
  clearFrame();
  showFrame();

  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println(F("Wi-Fi-modulet svarer ikke."));
    while (true) {
      showFailure();
      delay(500);
      clearFrame();
      showFrame();
      delay(500);
    }
  }

  connectToHotspot();
}

void loop() {
  static unsigned long lastRefresh = 0;
  static bool heartbeat = false;

  if (WiFi.status() != WL_CONNECTED) {
    connectToHotspot();
    return;
  }

  if (millis() - lastRefresh >= STATUS_REFRESH_MS) {
    lastRefresh = millis();
    heartbeat = !heartbeat;
    showConnected(WiFi.RSSI(), heartbeat);
  }
}
