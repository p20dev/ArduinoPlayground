#include <DallasTemperature.h>
#include <OneWire.h>
#include <WiFiS3.h>
#include <Wire.h>

#include "arduino_secrets.h"

constexpr uint8_t ONE_WIRE_PIN = 2;
constexpr uint8_t LCD_COLUMNS = 16;
constexpr uint8_t LCD_ROWS = 2;
constexpr uint8_t LCD_PIN_RS = 0x01;
constexpr uint8_t LCD_PIN_ENABLE = 0x04;
constexpr uint8_t LCD_PIN_BACKLIGHT = 0x08;
constexpr unsigned long MEASUREMENT_INTERVAL_MS = 10000;
constexpr unsigned long UPLOAD_INTERVAL_MS = 5UL * 60UL * 1000UL;
constexpr unsigned long UPLOAD_RETRY_MS = 60000;
constexpr unsigned long BACKLOG_UPLOAD_INTERVAL_MS = 10000;
constexpr unsigned long WIFI_RETRY_MS = 15000;
constexpr unsigned long TIME_RETRY_MS = 60000;
constexpr unsigned long TIME_SYNC_INTERVAL_MS = 6UL * 60UL * 60UL * 1000UL;
constexpr unsigned long RESPONSE_TIMEOUT_MS = 5000;
constexpr uint16_t BUFFER_CAPACITY = 60U * 60U / 10U;
constexpr uint8_t BATCH_SIZE = 30;
constexpr size_t REQUEST_BODY_CAPACITY = 3072;
constexpr uint8_t WIFI_RECOVERY_AFTER_FAILURES = 3;
constexpr uint8_t BOARD_RESTART_AFTER_FAILURES = 12;

constexpr char API_HOST[] = "payloads.p20.dev";
constexpr uint16_t API_PORT = 443;
constexpr char API_PATH[] = "/api/payloads";
constexpr char DEVICE_ID[] = "uno-r4-wifi-ds18b20";

OneWire oneWire(ONE_WIRE_PIN);
DallasTemperature temperatureSensors(&oneWire);
WiFiUDP timeUdp;
uint8_t lcdAddress = 0;
uint8_t lcdBacklightState = LCD_PIN_BACKLIGHT;

// Separate fixed-size arrays avoid heap fragmentation and use 3,600 bytes.
uint32_t measuredAtUnix[BUFFER_CAPACITY] = {};
uint32_t measuredAtMillis[BUFFER_CAPACITY] = {};
int16_t temperatureCentiDegrees[BUFFER_CAPACITY] = {};
char requestBody[REQUEST_BODY_CAPACITY] = {};

uint16_t bufferHead = 0;
uint16_t bufferCount = 0;
uint32_t droppedMeasurements = 0;
uint8_t consecutiveUploadFailures = 0;

unsigned long lastMeasurementAt = 0;
unsigned long lastUploadAttemptAt = 0;
unsigned long lastWiFiAttemptAt = 0;
unsigned long lastTimeAttemptAt = 0;
unsigned long clockSyncedAtMillis = 0;
uint32_t clockSyncedAtUnix = 0;

enum class DisplayEvent : uint8_t {
  NORMAL, UPLOADING, UPLOAD_SUCCESS, UPLOAD_ERROR, SENSOR_ERROR, RESTARTING
};

DisplayEvent displayEvent = DisplayEvent::NORMAL;
unsigned long displayEventUntil = 0;
unsigned long lastDisplayRefreshAt = 0;
unsigned long lastLcdScanAt = 0;
int16_t latestTemperatureCentiDegrees = 0;
bool hasTemperature = false;
int lastHttpStatus = 0;

void lcdExpanderWrite(uint8_t value) {
  Wire.beginTransmission(lcdAddress);
  Wire.write(value | lcdBacklightState);
  Wire.endTransmission();
}

void lcdPulseEnable(uint8_t value) {
  lcdExpanderWrite(value | LCD_PIN_ENABLE);
  delayMicroseconds(1);
  lcdExpanderWrite(value & ~LCD_PIN_ENABLE);
  delayMicroseconds(50);
}

void lcdWriteNibble(uint8_t nibble, bool dataMode) {
  uint8_t value = (nibble & 0x0F) << 4;
  if (dataMode) value |= LCD_PIN_RS;
  lcdExpanderWrite(value);
  lcdPulseEnable(value);
}

void lcdSend(uint8_t value, bool dataMode) {
  lcdWriteNibble(value >> 4, dataMode);
  lcdWriteNibble(value, dataMode);
}

void lcdCommand(uint8_t command) {
  lcdSend(command, false);
  if (command == 0x01 || command == 0x02) delayMicroseconds(2000);
}

void lcdWrite(uint8_t value) {
  lcdSend(value, true);
}

void lcdSetCursor(uint8_t column, uint8_t row) {
  static const uint8_t rowOffset[] = {0x00, 0x40};
  lcdCommand(0x80 | (column + rowOffset[row % LCD_ROWS]));
}

void lcdPrintLine(uint8_t row, const char *text) {
  lcdSetCursor(0, row);
  uint8_t written = 0;
  while (*text && written < LCD_COLUMNS) {
    lcdWrite(*text++);
    ++written;
  }
  while (written++ < LCD_COLUMNS) lcdWrite(' ');
}

uint8_t findLcdDisplay() {
  const uint8_t ranges[][2] = {{0x20, 0x27}, {0x38, 0x3F}};
  for (const auto &range : ranges) {
    for (uint8_t address = range[0]; address <= range[1]; ++address) {
      Wire.beginTransmission(address);
      if (Wire.endTransmission() == 0) return address;
    }
  }
  return 0;
}

void lcdInit() {
  delay(50);
  lcdWriteNibble(0x03, false); delayMicroseconds(4500);
  lcdWriteNibble(0x03, false); delayMicroseconds(4500);
  lcdWriteNibble(0x03, false); delayMicroseconds(150);
  lcdWriteNibble(0x02, false);
  lcdCommand(0x28);
  lcdCommand(0x08);
  lcdCommand(0x01);
  lcdCommand(0x06);
  lcdCommand(0x0C);
}

void formatTemperature(char *output, size_t outputSize) {
  if (!hasTemperature) {
    snprintf(output, outputSize, "--.--");
    return;
  }
  const uint16_t absolute = static_cast<uint16_t>(abs(latestTemperatureCentiDegrees));
  snprintf(output, outputSize, "%s%u.%02u", latestTemperatureCentiDegrees < 0 ? "-" : "",
      absolute / 100, absolute % 100);
}

void renderLcdStatus() {
  char line1[LCD_COLUMNS + 1];
  char line2[LCD_COLUMNS + 1];
  char temperature[8];
  formatTemperature(temperature, sizeof(temperature));

  switch (displayEvent) {
    case DisplayEvent::NORMAL:
      snprintf(line1, sizeof(line1), "T:%s W%c U%c", temperature,
          WiFi.status() == WL_CONNECTED ? '+' : '-', clockSyncedAtUnix != 0 ? '+' : '-');
      snprintf(line2, sizeof(line2), "Buf:%03u/%03u %c", bufferCount, BUFFER_CAPACITY,
          (millis() / 500UL) % 2 == 0 ? '*' : ' ');
      break;
    case DisplayEvent::UPLOADING:
      snprintf(line1, sizeof(line1), "Uploader batch");
      snprintf(line2, sizeof(line2), "30 stk Buf:%03u", bufferCount);
      break;
    case DisplayEvent::UPLOAD_SUCCESS:
      snprintf(line1, sizeof(line1), "Upload OK HTTP%d", lastHttpStatus);
      snprintf(line2, sizeof(line2), "Buffer:%03u", bufferCount);
      break;
    case DisplayEvent::UPLOAD_ERROR:
      snprintf(line1, sizeof(line1), "UPLOAD FEJL %d", lastHttpStatus);
      snprintf(line2, sizeof(line2), "Forsoeg:%u/12", consecutiveUploadFailures);
      break;
    case DisplayEvent::SENSOR_ERROR:
      snprintf(line1, sizeof(line1), "SENSORFEJL");
      snprintf(line2, sizeof(line2), "Kontroller DS18B20");
      break;
    case DisplayEvent::RESTARTING:
      snprintf(line1, sizeof(line1), "GENSTARTER...");
      snprintf(line2, sizeof(line2), "Uploadfejl:%u", consecutiveUploadFailures);
      break;
  }
  lcdPrintLine(0, line1);
  lcdPrintLine(1, line2);
}

void updateDisplay(bool force = false) {
  if (!force && millis() - lastDisplayRefreshAt < 500) return;
  lastDisplayRefreshAt = millis();

  if (lcdAddress == 0) {
    if (millis() - lastLcdScanAt < 5000) return;
    lastLcdScanAt = millis();
    lcdAddress = findLcdDisplay();
    if (lcdAddress == 0) return;
    Serial.print(F("LCD1602 fundet paa I2C-adresse 0x"));
    Serial.println(lcdAddress, HEX);
    lcdInit();
  }

  if (displayEvent != DisplayEvent::NORMAL && displayEvent != DisplayEvent::UPLOADING &&
      static_cast<long>(millis() - displayEventUntil) >= 0) {
    displayEvent = DisplayEvent::NORMAL;
  }
  renderLcdStatus();
}

void setDisplayEvent(DisplayEvent event, unsigned long durationMs = 0) {
  displayEvent = event;
  displayEventUntil = durationMs == 0 ? 0 : millis() + durationMs;
  updateDisplay(true);
}

void connectWiFi() {
  if (WiFi.status() == WL_CONNECTED) return;
  if (millis() - lastWiFiAttemptAt < WIFI_RETRY_MS) return;

  lastWiFiAttemptAt = millis();
  WiFi.disconnect();
  Serial.print(F("Forbinder til Wi-Fi: "));
  Serial.println(SECRET_SSID);
  WiFi.begin(SECRET_SSID, SECRET_PASS);
}

uint32_t currentUnixTime() {
  if (clockSyncedAtUnix == 0) return 0;
  return clockSyncedAtUnix + (millis() - clockSyncedAtMillis) / 1000UL;
}

uint32_t requestNtpTime() {
  constexpr uint16_t NTP_LOCAL_PORT = 2390;
  constexpr uint16_t NTP_SERVER_PORT = 123;
  constexpr uint8_t NTP_PACKET_SIZE = 48;
  constexpr uint32_t NTP_TO_UNIX_OFFSET = 2208988800UL;
  const IPAddress timeServer(162, 159, 200, 123);
  uint8_t packet[NTP_PACKET_SIZE] = {};

  // Values used by the WiFiS3 board package's official NTP example.
  packet[0] = 0b11100011;
  packet[1] = 0;
  packet[2] = 6;
  packet[3] = 0xEC;
  packet[12] = 49;
  packet[13] = 0x4E;
  packet[14] = 49;
  packet[15] = 52;

  timeUdp.stop();
  if (!timeUdp.begin(NTP_LOCAL_PORT)) return 0;
  if (!timeUdp.beginPacket(timeServer, NTP_SERVER_PORT)) return 0;
  timeUdp.write(packet, sizeof(packet));
  if (!timeUdp.endPacket()) return 0;

  const unsigned long startedAt = millis();
  while (millis() - startedAt < 2000) {
    const int packetSize = timeUdp.parsePacket();
    if (packetSize >= NTP_PACKET_SIZE) {
      timeUdp.read(packet, sizeof(packet));
      const uint32_t secondsSince1900 =
          (static_cast<uint32_t>(packet[40]) << 24) |
          (static_cast<uint32_t>(packet[41]) << 16) |
          (static_cast<uint32_t>(packet[42]) << 8) |
          static_cast<uint32_t>(packet[43]);
      timeUdp.stop();
      if (secondsSince1900 <= NTP_TO_UNIX_OFFSET) return 0;
      return secondsSince1900 - NTP_TO_UNIX_OFFSET;
    }
    updateDisplay();
    delay(10);
  }

  timeUdp.stop();
  return 0;
}

void backfillUnsyncedTimestamps() {
  for (uint16_t offset = 0; offset < bufferCount; ++offset) {
    const uint16_t index = (bufferHead + offset) % BUFFER_CAPACITY;
    if (measuredAtUnix[index] == 0) {
      measuredAtUnix[index] = clockSyncedAtUnix -
          (clockSyncedAtMillis - measuredAtMillis[index]) / 1000UL;
    }
  }
}

void syncClock() {
  if (WiFi.status() != WL_CONNECTED) return;
  const bool clockIsDue = clockSyncedAtUnix == 0 ||
      millis() - clockSyncedAtMillis >= TIME_SYNC_INTERVAL_MS;
  if (!clockIsDue || millis() - lastTimeAttemptAt < TIME_RETRY_MS) return;

  lastTimeAttemptAt = millis();
  const unsigned long unixTime = requestNtpTime();
  if (unixTime < 1700000000UL) {
    Serial.println(F("Kunne ikke hente gyldig UTC-tid; proever igen senere."));
    return;
  }

  clockSyncedAtUnix = unixTime;
  clockSyncedAtMillis = millis();
  backfillUnsyncedTimestamps();
  Serial.print(F("UTC-tid synkroniseret: "));
  Serial.println(clockSyncedAtUnix);
}

void storeMeasurement(float celsius) {
  if (bufferCount == BUFFER_CAPACITY) {
    bufferHead = (bufferHead + 1) % BUFFER_CAPACITY;
    --bufferCount;
    ++droppedMeasurements;
    Serial.println(F("Maelebufferen er fuld; aeldste maaling blev overskrevet."));
  }

  const uint16_t index = (bufferHead + bufferCount) % BUFFER_CAPACITY;
  measuredAtUnix[index] = currentUnixTime();
  measuredAtMillis[index] = millis();
  temperatureCentiDegrees[index] = static_cast<int16_t>(roundf(celsius * 100.0f));
  ++bufferCount;
}

void takeMeasurement() {
  temperatureSensors.requestTemperatures();
  const float celsius = temperatureSensors.getTempCByIndex(0);
  if (celsius == DEVICE_DISCONNECTED_C) {
    Serial.println(F("DS18B20 blev ikke fundet paa 1-Wire-bussen."));
    setDisplayEvent(DisplayEvent::SENSOR_ERROR, 2000);
    return;
  }

  storeMeasurement(celsius);
  latestTemperatureCentiDegrees = static_cast<int16_t>(roundf(celsius * 100.0f));
  hasTemperature = true;
  Serial.print(F("Temperatur gemt: "));
  Serial.print(celsius, 2);
  Serial.print(F(" C; maalinger i buffer: "));
  Serial.println(bufferCount);
}

bool appendText(size_t &length, const char *text) {
  const size_t textLength = strlen(text);
  if (length + textLength >= REQUEST_BODY_CAPACITY) return false;
  memcpy(requestBody + length, text, textLength);
  length += textLength;
  requestBody[length] = '\0';
  return true;
}

bool buildBatchBody(uint8_t sampleCount, uint32_t batchId, size_t &bodyLength) {
  bodyLength = 0;
  char fragment[128];
  snprintf(fragment, sizeof(fragment),
      "{\"topic\":\"sensors/temperature\",\"payload\":{\"device\":\"%s\","
      "\"batchId\":%lu,\"samples\":[",
      DEVICE_ID, static_cast<unsigned long>(batchId));
  if (!appendText(bodyLength, fragment)) return false;

  for (uint8_t offset = 0; offset < sampleCount; ++offset) {
    const uint16_t index = (bufferHead + offset) % BUFFER_CAPACITY;
    const int16_t centiDegrees = temperatureCentiDegrees[index];
    const uint16_t absoluteTemperature = static_cast<uint16_t>(abs(centiDegrees));
    snprintf(fragment, sizeof(fragment),
        "%s{\"measuredAtUnix\":%lu,\"temperatureC\":%s%u.%02u}",
        offset == 0 ? "" : ",", static_cast<unsigned long>(measuredAtUnix[index]),
        centiDegrees < 0 ? "-" : "", absoluteTemperature / 100,
        absoluteTemperature % 100);
    if (!appendText(bodyLength, fragment)) return false;
  }

  return appendText(bodyLength, "]}}\n");
}

int readHttpStatus(WiFiSSLClient &client) {
  unsigned long lastActivityAt = millis();
  while (!client.available() && client.connected() &&
         millis() - lastActivityAt < RESPONSE_TIMEOUT_MS) {
    updateDisplay();
    delay(10);
  }
  if (!client.available()) return 0;

  char statusLine[64] = {};
  const size_t statusLength = client.readBytesUntil('\n', statusLine, sizeof(statusLine) - 1);
  statusLine[statusLength] = '\0';
  int statusCode = 0;
  sscanf(statusLine, "HTTP/%*s %d", &statusCode);

  // Drain headers and response body before releasing the socket.
  lastActivityAt = millis();
  while ((client.connected() || client.available()) &&
         millis() - lastActivityAt < RESPONSE_TIMEOUT_MS) {
    while (client.available()) {
      client.read();
      lastActivityAt = millis();
    }
    updateDisplay();
    delay(1);
  }
  return statusCode;
}

bool postOldestBatch(uint8_t sampleCount) {
  if (clockSyncedAtUnix == 0) {
    Serial.println(F("Upload udskudt: UTC-tiden er endnu ikke synkroniseret."));
    return false;
  }

  size_t bodyLength = 0;
  // The oldest sample's timestamp remains stable across retries and board restarts.
  const uint32_t batchId = measuredAtUnix[bufferHead];
  if (!buildBatchBody(sampleCount, batchId, bodyLength)) {
    Serial.println(F("Batchen kunne ikke vaere i request-bufferen."));
    return false;
  }

  WiFiSSLClient client;
  if (!client.connect(API_HOST, API_PORT)) {
    Serial.println(F("Kunne ikke oprette HTTPS-forbindelse til payload-serveren."));
    return false;
  }

  client.print(F("POST ")); client.print(API_PATH); client.println(F(" HTTP/1.1"));
  client.print(F("Host: ")); client.println(API_HOST);
  client.print(F("Authorization: Bearer ")); client.println(SECRET_API_TOKEN);
  client.println(F("Content-Type: application/json"));
  client.print(F("X-Device-ID: ")); client.println(DEVICE_ID);
  client.println(F("X-Client-Name: ds18b20-payload-sketch"));
  client.print(F("Content-Length: ")); client.println(bodyLength);
  client.println(F("Connection: close"));
  client.println();
  client.write(reinterpret_cast<const uint8_t *>(requestBody), bodyLength);

  const int statusCode = readHttpStatus(client);
  lastHttpStatus = statusCode;
  client.stop();
  Serial.print(F("Serverens HTTP-status: "));
  Serial.println(statusCode);
  if (statusCode < 200 || statusCode >= 300) return false;

  bufferHead = (bufferHead + sampleCount) % BUFFER_CAPACITY;
  bufferCount -= sampleCount;
  consecutiveUploadFailures = 0;
  Serial.print(F("Batch sendt; maalinger tilbage i buffer: "));
  Serial.println(bufferCount);
  return true;
}

void uploadWhenDue() {
  if (bufferCount < BATCH_SIZE || WiFi.status() != WL_CONNECTED ||
      clockSyncedAtUnix == 0) return;
  const unsigned long regularInterval = consecutiveUploadFailures == 0
      ? UPLOAD_INTERVAL_MS
      : UPLOAD_RETRY_MS;
  const bool regularBatchDue = bufferCount >= BATCH_SIZE &&
      millis() - lastUploadAttemptAt >= regularInterval;
  const bool backlogDue = consecutiveUploadFailures == 0 &&
      bufferCount >= BATCH_SIZE * 2U &&
      millis() - lastUploadAttemptAt >= BACKLOG_UPLOAD_INTERVAL_MS;
  if (!regularBatchDue && !backlogDue) return;

  lastUploadAttemptAt = millis();
  const uint8_t sampleCount = min(static_cast<uint16_t>(BATCH_SIZE), bufferCount);
  setDisplayEvent(DisplayEvent::UPLOADING);
  if (postOldestBatch(sampleCount)) {
    setDisplayEvent(DisplayEvent::UPLOAD_SUCCESS, 1000);
    return;
  }
  setDisplayEvent(DisplayEvent::UPLOAD_ERROR, 2000);

  ++consecutiveUploadFailures;
  Serial.print(F("Sammenhaengende uploadfejl: "));
  Serial.println(consecutiveUploadFailures);

  if (consecutiveUploadFailures % WIFI_RECOVERY_AFTER_FAILURES == 0) {
    Serial.println(F("Nulstiller Wi-Fi-forbindelsen efter uploadfejl."));
    WiFi.disconnect();
    lastWiFiAttemptAt = millis() - WIFI_RETRY_MS;
  }

  if (consecutiveUploadFailures >= BOARD_RESTART_AFTER_FAILURES) {
    Serial.println(F("Genstarter boardet efter vedvarende uploadfejl."));
    Serial.flush();
    setDisplayEvent(DisplayEvent::RESTARTING);
    const unsigned long restartAnimationStartedAt = millis();
    while (millis() - restartAnimationStartedAt < 800) {
      updateDisplay();
      delay(10);
    }
    NVIC_SystemReset();
  }
}

void setup() {
  Serial.begin(115200);
  Wire.begin();
  Wire.setClock(100000);
  temperatureSensors.begin();
  temperatureSensors.setResolution(10);
  lastLcdScanAt = millis() - 5000;
  setDisplayEvent(DisplayEvent::NORMAL);
  lastWiFiAttemptAt = millis() - WIFI_RETRY_MS;
  lastTimeAttemptAt = millis() - TIME_RETRY_MS;
  lastMeasurementAt = millis();
  lastUploadAttemptAt = millis();
  connectWiFi();
}

void loop() {
  connectWiFi();
  syncClock();

  const unsigned long elapsed = millis() - lastMeasurementAt;

  if (elapsed >= MEASUREMENT_INTERVAL_MS) {
    takeMeasurement();
    lastMeasurementAt += MEASUREMENT_INTERVAL_MS;
  }

  uploadWhenDue();
  updateDisplay();
  delay(20);
}
