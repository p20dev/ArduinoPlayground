#include <Arduino_LED_Matrix.h>
#include <DallasTemperature.h>
#include <OneWire.h>
#include <WiFiS3.h>

#include "arduino_secrets.h"

constexpr uint8_t ROWS = 8;
constexpr uint8_t COLUMNS = 12;
constexpr uint8_t ONE_WIRE_PIN = 2;
constexpr unsigned long PAYLOAD_INTERVAL_MS = 10000;
constexpr unsigned long WIFI_RETRY_MS = 3000;
constexpr unsigned long RESPONSE_TIMEOUT_MS = 5000;

constexpr char API_HOST[] = "payloads.p20.dev";
constexpr uint16_t API_PORT = 443;
constexpr char API_PATH[] = "/api/payloads";

ArduinoLEDMatrix matrix;
OneWire oneWire(ONE_WIRE_PIN);
DallasTemperature temperatureSensors(&oneWire);
uint8_t frame[ROWS][COLUMNS] = {};

unsigned long lastPayloadAt = 0;
unsigned long lastWiFiAttemptAt = 0;
int lastDisplayedSeconds = -1;
bool lastDisplayedConnected = false;

void clearFrame() {
  memset(frame, 0, sizeof(frame));
}

void setPixel(int x, int y, bool on = true) {
  if (x >= 0 && x < COLUMNS && y >= 0 && y < ROWS) {
    frame[y][x] = on ? 1 : 0;
  }
}

void showCountdown(uint8_t secondsRemaining, bool connected) {
  clearFrame();

  // Wi-Fi status: a check mark when connected, an X otherwise.
  if (connected) {
    setPixel(0, 2);
    setPixel(1, 3);
    setPixel(2, 2);
    setPixel(3, 1);
  } else {
    setPixel(0, 0);
    setPixel(1, 1);
    setPixel(2, 2);
    setPixel(2, 0);
    setPixel(0, 2);
  }

  // Ten LEDs count down from 10 to 1 before the next payload.
  const uint8_t lit = min(secondsRemaining, static_cast<uint8_t>(10));
  for (uint8_t column = 0; column < lit; ++column) {
    setPixel(column + 1, 6);
  }
  matrix.renderBitmap(frame, ROWS, COLUMNS);
}

void connectWiFi() {
  if (WiFi.status() == WL_CONNECTED) return;
  if (millis() - lastWiFiAttemptAt < WIFI_RETRY_MS) return;

  lastWiFiAttemptAt = millis();
  Serial.print(F("Forbinder til Wi-Fi: "));
  Serial.println(SECRET_SSID);
  WiFi.begin(SECRET_SSID, SECRET_PASS);
}

bool postTemperature(float celsius) {
  WiFiSSLClient client;
  if (!client.connect(API_HOST, API_PORT)) {
    Serial.println(F("Kunne ikke oprette HTTPS-forbindelse til payload-serveren."));
    return false;
  }

  String body = F("{\"topic\":\"sensors/temperature\",\"payload\":{");
  body += F("\"device\":\"uno-r4-wifi-ds18b20\",\"temperatureC\":");
  body += String(celsius, 2);
  body += F("}}");

  client.print(F("POST "));
  client.print(API_PATH);
  client.println(F(" HTTP/1.1"));
  client.print(F("Host: "));
  client.println(API_HOST);
  client.print(F("Authorization: Bearer "));
  client.println(SECRET_API_TOKEN);
  client.println(F("Content-Type: application/json"));
  client.println(F("X-Device-ID: uno-r4-wifi-ds18b20"));
  client.println(F("X-Client-Name: ds18b20-payload-sketch"));
  client.print(F("Content-Length: "));
  client.println(body.length());
  client.println(F("Connection: close"));
  client.println();
  client.print(body);

  const unsigned long startedAt = millis();
  while (!client.available() && client.connected() &&
         millis() - startedAt < RESPONSE_TIMEOUT_MS) {
    delay(10);
  }

  if (!client.available()) {
    Serial.println(F("Payload-serveren svarede ikke inden timeout."));
    client.stop();
    return false;
  }

  const String statusLine = client.readStringUntil('\n');
  const bool accepted = statusLine.indexOf(" 201 ") >= 0;
  Serial.print(F("Serverens svar: "));
  Serial.println(statusLine);
  client.stop();
  return accepted;
}

void sendPayload() {
  temperatureSensors.requestTemperatures();
  const float celsius = temperatureSensors.getTempCByIndex(0);

  if (celsius == DEVICE_DISCONNECTED_C) {
    Serial.println(F("DS18B20 blev ikke fundet paa 1-Wire-bussen."));
    return;
  }

  Serial.print(F("Temperatur: "));
  Serial.print(celsius, 2);
  Serial.println(F(" C"));
  Serial.println(postTemperature(celsius) ? F("Payload sendt.") : F("Payload fejlede."));
}

void setup() {
  Serial.begin(115200);
  matrix.begin();
  temperatureSensors.begin();
  temperatureSensors.setResolution(10);

  showCountdown(10, false);
  lastWiFiAttemptAt = millis() - WIFI_RETRY_MS;
  connectWiFi();
  lastPayloadAt = millis();
}

void loop() {
  connectWiFi();

  const unsigned long elapsed = millis() - lastPayloadAt;
  const uint8_t secondsRemaining = elapsed >= PAYLOAD_INTERVAL_MS
      ? 0
      : static_cast<uint8_t>((PAYLOAD_INTERVAL_MS - elapsed + 999) / 1000);

  const bool connected = WiFi.status() == WL_CONNECTED;
  if (secondsRemaining != lastDisplayedSeconds || connected != lastDisplayedConnected) {
    lastDisplayedSeconds = secondsRemaining;
    lastDisplayedConnected = connected;
    showCountdown(secondsRemaining, connected);
  }

  if (elapsed >= PAYLOAD_INTERVAL_MS) {
    if (WiFi.status() == WL_CONNECTED) {
      sendPayload();
    } else {
      Serial.println(F("Payload sprunget over: Wi-Fi er ikke forbundet."));
    }
    lastPayloadAt = millis();
    lastDisplayedSeconds = -1;
  }

  delay(20);
}
