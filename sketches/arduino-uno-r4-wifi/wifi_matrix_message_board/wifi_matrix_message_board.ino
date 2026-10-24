#include <ArduinoGraphics.h>
#include <Arduino_LED_Matrix.h>
#include <TextAnimation.h>
#include <WiFiS3.h>

#if __has_include("arduino_secrets.h")
#include "arduino_secrets.h"
#define HAS_WIFI_SECRETS 1
#else
#define HAS_WIFI_SECRETS 0
#endif

constexpr char AP_SSID[] = "Arduino-Matrix";
constexpr char AP_PASSWORD[] = "matrix123";
constexpr uint8_t MAX_MESSAGE_LENGTH = 64;
constexpr uint16_t DEFAULT_SCROLL_SPEED_MS = 75;
constexpr uint16_t REPEAT_PAUSE_MS = 900;

// Font_5x7 uses six horizontal pixels per character including spacing. The
// extra frames let the text enter from and leave through the matrix edges.
constexpr size_t ANIMATION_FRAMES = MAX_MESSAGE_LENGTH * 6 + 24;
TEXT_ANIMATION_DEFINE(scrollAnimation, ANIMATION_FRAMES)

ArduinoLEDMatrix matrix;
WiFiServer server(80);

String message = "Arduino message board er klar";
uint16_t scrollSpeedMs = DEFAULT_SCROLL_SPEED_MS;
bool accessPointMode = false;
bool waitingToRestart = false;
unsigned long restartAt = 0;
volatile bool animationComplete = false;

void onAnimationComplete() {
  // This callback runs in an interrupt, so it must remain very short.
  animationComplete = true;
}

String ipToString(const IPAddress &ip) {
  return String(ip[0]) + "." + String(ip[1]) + "." +
         String(ip[2]) + "." + String(ip[3]);
}

String urlDecode(const String &encoded) {
  String decoded;
  decoded.reserve(encoded.length());

  for (unsigned int i = 0; i < encoded.length(); ++i) {
    const char current = encoded[i];

    if (current == '+') {
      decoded += ' ';
    } else if (current == '%' && i + 2 < encoded.length()) {
      const char hex[] = {encoded[i + 1], encoded[i + 2], '\0'};
      char *end = nullptr;
      const long value = strtol(hex, &end, 16);

      if (end == hex + 2) {
        decoded += static_cast<char>(value);
        i += 2;
      } else {
        decoded += current;
      }
    } else {
      decoded += current;
    }
  }

  return decoded;
}

String toMatrixText(const String &input) {
  String output;
  output.reserve(MAX_MESSAGE_LENGTH);

  for (unsigned int i = 0;
       i < input.length() && output.length() < MAX_MESSAGE_LENGTH;
       ++i) {
    const uint8_t current = static_cast<uint8_t>(input[i]);

    if (current == 0xC3 && i + 1 < input.length()) {
      const uint8_t next = static_cast<uint8_t>(input[++i]);
      const char *replacement = nullptr;

      switch (next) {
        case 0xA6: replacement = "ae"; break;  // ae
        case 0xB8: replacement = "oe"; break;  // oe
        case 0xA5: replacement = "aa"; break;  // aa
        case 0x86: replacement = "AE"; break;
        case 0x98: replacement = "OE"; break;
        case 0x85: replacement = "AA"; break;
      }

      if (replacement != nullptr) {
        for (uint8_t j = 0;
             replacement[j] != '\0' && output.length() < MAX_MESSAGE_LENGTH;
             ++j) {
          output += replacement[j];
        }
      } else {
        output += '?';
      }
    } else if (current >= 32 && current <= 126) {
      output += static_cast<char>(current);
    } else {
      output += '?';
    }
  }

  output.trim();
  return output;
}

String htmlEscape(String value) {
  value.replace("&", "&amp;");
  value.replace("<", "&lt;");
  value.replace(">", "&gt;");
  value.replace("\"", "&quot;");
  value.replace("'", "&#39;");
  return value;
}

String queryValue(const String &query, const String &name) {
  const String prefix = name + "=";
  int start = 0;

  while (start < static_cast<int>(query.length())) {
    int end = query.indexOf('&', start);
    if (end < 0) {
      end = query.length();
    }

    const String part = query.substring(start, end);
    if (part.startsWith(prefix)) {
      return urlDecode(part.substring(prefix.length()));
    }

    start = end + 1;
  }

  return "";
}

void startAnimation() {
  matrix.clear();
  animationComplete = false;
  waitingToRestart = false;

  matrix.beginDraw();
  matrix.stroke(0xFFFFFFFF);
  matrix.textFont(Font_5x7);
  matrix.textScrollSpeed(scrollSpeedMs);
  matrix.beginText(12, 1, 0xFFFFFF);
  matrix.print(message);
  matrix.endTextAnimation(SCROLL_LEFT, scrollAnimation);

  matrix.loadTextAnimationSequence(scrollAnimation);
  matrix.play();
}

void sendRedirect(WiFiClient &client) {
  client.println(F("HTTP/1.1 303 See Other"));
  client.println(F("Location: /"));
  client.println(F("Connection: close"));
  client.println();
}

void sendPage(WiFiClient &client) {
  const String escapedMessage = htmlEscape(message);
  const String ip = ipToString(WiFi.localIP());

  client.println(F("HTTP/1.1 200 OK"));
  client.println(F("Content-Type: text/html; charset=utf-8"));
  client.println(F("Cache-Control: no-store"));
  client.println(F("Connection: close"));
  client.println();

  client.println(F("<!doctype html><html lang='da'><head>"));
  client.println(F("<meta charset='utf-8'><meta name='viewport' content='width=device-width,initial-scale=1'>"));
  client.println(F("<title>Arduino Matrix</title><style>"));
  client.println(F(":root{color-scheme:dark}*{box-sizing:border-box}body{margin:0;min-height:100vh;display:grid;place-items:center;background:#101418;color:#f6f7f8;font:16px system-ui,sans-serif;padding:20px}.card{width:min(560px,100%);background:#1c232a;border:1px solid #34404a;border-radius:18px;padding:28px;box-shadow:0 18px 50px #0008}h1{margin:0 0 8px;color:#ff7a18;font-size:clamp(1.6rem,5vw,2.4rem)}p{color:#b9c3cc;line-height:1.5}label{display:block;margin:20px 0 7px;font-weight:700}input[type=text]{width:100%;padding:13px;border:1px solid #51606d;border-radius:10px;background:#0d1115;color:#fff;font:inherit}input[type=range]{width:100%;accent-color:#ff7a18}.row{display:flex;justify-content:space-between;color:#b9c3cc}button{width:100%;margin-top:24px;padding:14px;border:0;border-radius:10px;background:#ff7a18;color:#17100a;font:700 1rem system-ui;cursor:pointer}.meta{margin-top:22px;padding-top:18px;border-top:1px solid #34404a;font-size:.9rem}code{color:#ffd0ad}</style></head><body>"));
  client.println(F("<main class='card'><h1>LED Matrix Message Board</h1>"));
  client.println(F("<p>Skriv en besked, og send den direkte til Arduinoens 12&times;8 LED-matrix.</p>"));
  client.println(F("<form action='/set' method='get'><label for='message'>Besked</label>"));
  client.print(F("<input id='message' name='message' type='text' maxlength='64' required value=\""));
  client.print(escapedMessage);
  client.println(F("\">"));
  client.println(F("<label for='speed'>Hastighed</label><div class='row'><span>Hurtig</span><output id='speedValue'></output><span>Rolig</span></div>"));
  client.print(F("<input id='speed' name='speed' type='range' min='35' max='180' step='5' value='"));
  client.print(scrollSpeedMs);
  client.println(F("' oninput=\"speedValue.value=this.value+' ms'\">"));
  client.println(F("<button type='submit'>Vis på matrix</button></form>"));
  client.print(F("<p class='meta'>Netværk: <code>"));
  client.print(accessPointMode ? AP_SSID : WiFi.SSID());
  client.print(F("</code><br>Adresse: <code>http://"));
  client.print(ip);
  client.println(F("</code></p></main><script>speedValue.value=speed.value+' ms'</script></body></html>"));
}

void handleClient() {
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  client.setTimeout(750);
  String requestLine = client.readStringUntil('\n');
  requestLine.trim();

  while (client.connected()) {
    String headerLine = client.readStringUntil('\n');
    headerLine.trim();
    if (headerLine.length() == 0) {
      break;
    }
  }

  if (requestLine.startsWith("GET /set?")) {
    const int targetEnd = requestLine.indexOf(' ', 4);
    const int queryStart = requestLine.indexOf('?');

    if (targetEnd > queryStart) {
      const String query = requestLine.substring(queryStart + 1, targetEnd);
      const String requestedMessage = toMatrixText(queryValue(query, "message"));
      const long requestedSpeed = queryValue(query, "speed").toInt();

      if (requestedMessage.length() > 0) {
        message = requestedMessage;
      }
      if (requestedSpeed >= 35 && requestedSpeed <= 180) {
        scrollSpeedMs = requestedSpeed;
      }

      startAnimation();
    }

    sendRedirect(client);
  } else {
    sendPage(client);
  }

  delay(1);
  client.stop();
}

bool connectToConfiguredWiFi() {
#if HAS_WIFI_SECRETS
  Serial.print(F("Forbinder til "));
  Serial.println(SECRET_SSID);
  WiFi.begin(SECRET_SSID, SECRET_PASS);

  const unsigned long deadline = millis() + 15000;
  while (WiFi.status() != WL_CONNECTED &&
         static_cast<long>(millis() - deadline) < 0) {
    delay(250);
    Serial.print('.');
  }
  Serial.println();

  return WiFi.status() == WL_CONNECTED;
#else
  return false;
#endif
}

bool startAccessPoint() {
  accessPointMode = true;
  Serial.print(F("Opretter access point: "));
  Serial.println(AP_SSID);
  return WiFi.beginAP(AP_SSID, AP_PASSWORD) == WL_AP_LISTENING;
}

void setup() {
  Serial.begin(115200);
  const unsigned long serialDeadline = millis() + 2500;
  while (!Serial && static_cast<long>(millis() - serialDeadline) < 0) {
  }

  matrix.begin();
  matrix.setCallback(onAnimationComplete);
  startAnimation();

  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println(F("Wi-Fi-modulet svarer ikke."));
    while (true) {
      delay(1000);
    }
  }

  if (!connectToConfiguredWiFi()) {
    WiFi.disconnect();
    delay(500);

    if (!startAccessPoint()) {
      Serial.println(F("Kunne ikke starte access point."));
      while (true) {
        delay(1000);
      }
    }
  }

  server.begin();

  Serial.println(F("Message board er klar."));
  Serial.print(F("Aabn http://"));
  Serial.println(ipToString(WiFi.localIP()));
  if (accessPointMode) {
    Serial.print(F("Wi-Fi-kode: "));
    Serial.println(AP_PASSWORD);
  }
}

void loop() {
  handleClient();

  if (animationComplete) {
    noInterrupts();
    animationComplete = false;
    interrupts();

    waitingToRestart = true;
    restartAt = millis() + REPEAT_PAUSE_MS;
  }

  if (waitingToRestart && static_cast<long>(millis() - restartAt) >= 0) {
    waitingToRestart = false;
    matrix.play();
  }
}

