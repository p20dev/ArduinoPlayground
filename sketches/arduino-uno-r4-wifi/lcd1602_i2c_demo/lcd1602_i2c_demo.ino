#include <Wire.h>

// HD44780 16x2 display with the common PCF8574 I2C backpack mapping:
// P0=RS, P1=RW, P2=Enable, P3=backlight, P4..P7=D4..D7.
constexpr uint8_t LCD_COLUMNS = 16;
constexpr uint8_t LCD_ROWS = 2;
constexpr uint8_t PIN_RS = 0x01;
constexpr uint8_t PIN_ENABLE = 0x04;
constexpr uint8_t PIN_BACKLIGHT = 0x08;

uint8_t lcdAddress = 0;
uint8_t backlightState = PIN_BACKLIGHT;

const byte heart[8] = {
  B00000, B01010, B11111, B11111, B01110, B00100, B00000, B00000
};

const byte smile[8] = {
  B00000, B01010, B00000, B00000, B10001, B01110, B00000, B00000
};

const byte rocket[8] = {
  B00100, B01110, B01110, B11111, B10101, B00100, B01010, B10001
};

const byte spinner[4][8] = {
  {B00100, B00100, B00100, B00100, B00100, B00100, B00100, B00000},
  {B00000, B00000, B00001, B00010, B00100, B01000, B10000, B00000},
  {B00000, B00000, B00000, B11111, B00000, B00000, B00000, B00000},
  {B00000, B00000, B10000, B01000, B00100, B00010, B00001, B00000}
};

void expanderWrite(uint8_t value) {
  Wire.beginTransmission(lcdAddress);
  Wire.write(value | backlightState);
  Wire.endTransmission();
}

void pulseEnable(uint8_t value) {
  expanderWrite(value | PIN_ENABLE);
  delayMicroseconds(1);
  expanderWrite(value & ~PIN_ENABLE);
  delayMicroseconds(50);
}

void writeNibble(uint8_t nibble, bool dataMode) {
  uint8_t value = (nibble & 0x0F) << 4;
  if (dataMode) value |= PIN_RS;
  expanderWrite(value);
  pulseEnable(value);
}

void lcdSend(uint8_t value, bool dataMode) {
  writeNibble(value >> 4, dataMode);
  writeNibble(value, dataMode);
}

void lcdCommand(uint8_t command) {
  lcdSend(command, false);
  if (command == 0x01 || command == 0x02) delayMicroseconds(2000);
}

void lcdWrite(uint8_t value) {
  lcdSend(value, true);
}

void lcdPrint(const char *text) {
  while (*text) lcdWrite(*text++);
}

void lcdClear() {
  lcdCommand(0x01);
}

void lcdSetCursor(uint8_t column, uint8_t row) {
  static const uint8_t rowOffset[] = {0x00, 0x40};
  lcdCommand(0x80 | (column + rowOffset[row % LCD_ROWS]));
}

void lcdCreateChar(uint8_t slot, const byte bitmap[8]) {
  lcdCommand(0x40 | ((slot & 0x07) << 3));
  for (uint8_t row = 0; row < 8; ++row) lcdWrite(bitmap[row]);
}

void lcdInit() {
  delay(50);
  writeNibble(0x03, false);
  delayMicroseconds(4500);
  writeNibble(0x03, false);
  delayMicroseconds(4500);
  writeNibble(0x03, false);
  delayMicroseconds(150);
  writeNibble(0x02, false);

  lcdCommand(0x28);  // 4-bit, two lines, 5x8 font
  lcdCommand(0x08);  // Display off
  lcdClear();
  lcdCommand(0x06);  // Cursor moves right
  lcdCommand(0x0C);  // Display on, cursor off
}

uint8_t findDisplay() {
  // PCF8574 and PCF8574A address ranges.
  const uint8_t ranges[][2] = {{0x20, 0x27}, {0x38, 0x3F}};
  for (const auto &range : ranges) {
    for (uint8_t address = range[0]; address <= range[1]; ++address) {
      Wire.beginTransmission(address);
      if (Wire.endTransmission() == 0) return address;
    }
  }
  return 0;
}

void printPadded(const char *text, uint8_t width = LCD_COLUMNS) {
  uint8_t count = 0;
  while (*text && count < width) {
    lcdWrite(*text++);
    ++count;
  }
  while (count++ < width) lcdWrite(' ');
}

void bootSequence() {
  lcdClear();
  lcdSetCursor(0, 0);
  lcdWrite(2);
  lcdPrint(" R4 WiFi LAB ");
  lcdWrite(2);

  lcdSetCursor(0, 1);
  for (uint8_t i = 0; i < LCD_COLUMNS; ++i) {
    lcdWrite(0xFF);
    delay(75);
  }
  delay(350);

  for (uint8_t flash = 0; flash < 2; ++flash) {
    backlightState = 0;
    expanderWrite(0);
    delay(90);
    backlightState = PIN_BACKLIGHT;
    expanderWrite(0);
    delay(120);
  }
}

void messageCardsDemo() {
  const char *cards[][2] = {
    {" ARDUINO UNO R4", "  WIFI POWERED"},
    {"  LCD1602+I2C", " PIXELS AT WORK"},
    {"   CODE. MAKE.", "    REPEAT!"}
  };

  lcdClear();
  lcdSetCursor(0, 0);
  printPadded(" MAKER MODE");
  delay(900);

  for (const auto &card : cards) {
    lcdSetCursor(0, 0);
    printPadded(card[0]);
    lcdSetCursor(0, 1);
    printPadded(card[1]);
    delay(1350);
  }
}

void equalizerDemo() {
  for (uint8_t level = 0; level < 8; ++level) {
    byte bar[8];
    for (uint8_t row = 0; row < 8; ++row) {
      bar[row] = row >= (7 - level) ? B11111 : B00000;
    }
    lcdCreateChar(level, bar);
  }

  lcdClear();
  lcdSetCursor(0, 0);
  printPadded("  BYTE BEATS");

  for (uint8_t frame = 0; frame < 38; ++frame) {
    lcdSetCursor(0, 1);
    for (uint8_t column = 0; column < LCD_COLUMNS; ++column) {
      // A deterministic animated wave: no random seed needed.
      uint8_t level = (column * 3 + frame * 2 + (column ^ frame)) % 8;
      lcdWrite(level);
    }
    delay(95);
  }

  // Restore the icons overwritten by the equalizer characters.
  lcdCreateChar(0, heart);
  lcdCreateChar(1, smile);
  lcdCreateChar(2, rocket);
}

void bouncingPixelDemo() {
  lcdClear();
  lcdSetCursor(0, 0);
  printPadded(" BOUNCING PIXEL");
  lcdSetCursor(0, 1);
  printPadded("     80x16");
  delay(900);
  lcdClear();

  int x = 2;
  int y = 2;
  int dx = 2;
  int dy = 1;
  int previousColumn = -1;
  int previousRow = -1;

  for (uint8_t frame = 0; frame < 150; ++frame) {
    if (previousColumn >= 0) {
      lcdSetCursor(previousColumn, previousRow);
      lcdWrite(' ');
    }

    byte pixel[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    pixel[y % 8] = 1 << (4 - (x % 5));
    lcdCreateChar(4, pixel);

    int column = x / 5;
    int row = y / 8;
    lcdSetCursor(column, row);
    lcdWrite(4);
    previousColumn = column;
    previousRow = row;

    x += dx;
    y += dy;
    if (x <= 0 || x >= 79) {
      x = constrain(x, 0, 79);
      dx = -dx;
    }
    if (y <= 0 || y >= 15) {
      y = constrain(y, 0, 15);
      dy = -dy;
    }
    delay(35);
  }
}

void pixelRainDemo() {
  lcdClear();
  lcdSetCursor(0, 0);
  printPadded("   PIXEL RAIN");
  lcdSetCursor(0, 1);
  printPadded("  //////\\\\\\");
  delay(900);

  lcdClear();
  for (uint8_t row = 0; row < LCD_ROWS; ++row) {
    lcdSetCursor(0, row);
    for (uint8_t column = 0; column < LCD_COLUMNS; ++column) {
      lcdWrite((column + row * 3) % 8);
    }
  }

  for (uint8_t frame = 0; frame < 55; ++frame) {
    for (uint8_t slot = 0; slot < 8; ++slot) {
      byte rain[8] = {0, 0, 0, 0, 0, 0, 0, 0};
      for (uint8_t pixelColumn = 0; pixelColumn < 5; ++pixelColumn) {
        uint8_t head = (frame + slot * 3 + pixelColumn * 5) % 13;
        if (head < 8) rain[head] |= 1 << (4 - pixelColumn);
        if (head > 0 && head - 1 < 8 && ((slot + pixelColumn) & 1)) {
          rain[head - 1] |= 1 << (4 - pixelColumn);
        }
      }
      lcdCreateChar(slot, rain);
    }
    delay(80);
  }

  lcdCreateChar(0, heart);
  lcdCreateChar(1, smile);
  lcdCreateChar(2, rocket);
}

void uptimeDemo() {
  lcdClear();
  lcdSetCursor(0, 0);
  lcdPrint("SYSTEM ONLINE ");
  lcdWrite(1);

  for (uint8_t frame = 0; frame < 32; ++frame) {
    lcdCreateChar(3, spinner[frame % 4]);
    unsigned long seconds = millis() / 1000;
    char line[17];
    snprintf(line, sizeof(line), "UP %5lus     ", seconds);
    lcdSetCursor(0, 1);
    lcdWrite(3);
    lcdWrite(' ');
    printPadded(line, 14);
    delay(125);
  }
}

void setup() {
  Serial.begin(115200);
  Wire.begin();
  Wire.setClock(100000);

  lcdAddress = findDisplay();
  if (lcdAddress == 0) {
    Serial.println("LCD1602-demo: intet PCF8574 I2C-display fundet.");
    Serial.println("Kontroller GND, 5V, SDA og SCL, og genstart boardet.");
    return;
  }

  Serial.print("LCD1602 fundet paa I2C-adresse 0x");
  Serial.println(lcdAddress, HEX);

  lcdInit();
  lcdCreateChar(0, heart);
  lcdCreateChar(1, smile);
  lcdCreateChar(2, rocket);
  bootSequence();
}

void loop() {
  if (lcdAddress == 0) {
    delay(1000);
    return;
  }

  messageCardsDemo();
  equalizerDemo();
  bouncingPixelDemo();
  pixelRainDemo();
  uptimeDemo();
}
