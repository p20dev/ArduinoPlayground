#include <Arduino_LED_Matrix.h>

// Arduino UNO R4 WiFi LED matrix demo (12 columns x 8 rows).
// The effects run continuously and require no additional hardware.

constexpr uint8_t ROWS = 8;
constexpr uint8_t COLS = 12;

ArduinoLEDMatrix matrix;
uint8_t frame[ROWS][COLS] = {};

void clearFrame() {
  memset(frame, 0, sizeof(frame));
}

void setPixel(int x, int y, bool on = true) {
  if (x >= 0 && x < COLS && y >= 0 && y < ROWS) {
    frame[y][x] = on ? 1 : 0;
  }
}

void showFrame(unsigned long durationMs) {
  matrix.renderBitmap(frame, ROWS, COLS);
  delay(durationMs);
}

void pauseBetweenEffects() {
  clearFrame();
  showFrame(250);
}

void scannerEffect() {
  for (int x = -2; x < COLS + 2; ++x) {
    clearFrame();

    for (int y = 0; y < ROWS; ++y) {
      setPixel(x, y);
      if ((y & 1) == 0) {
        setPixel(x - 1, y);
      }
    }

    showFrame(65);
  }

  for (int x = COLS + 1; x >= -2; --x) {
    clearFrame();

    for (int y = 0; y < ROWS; ++y) {
      setPixel(x, y);
      if ((y & 1) != 0) {
        setPixel(x + 1, y);
      }
    }

    showFrame(65);
  }
}

void rainEffect() {
  int8_t drops[COLS];

  for (uint8_t x = 0; x < COLS; ++x) {
    drops[x] = random(-10, 1);
  }

  for (uint8_t step = 0; step < 75; ++step) {
    clearFrame();

    for (uint8_t x = 0; x < COLS; ++x) {
      setPixel(x, drops[x]);
      setPixel(x, drops[x] - 2);

      ++drops[x];
      if (drops[x] > ROWS + random(0, 5)) {
        drops[x] = random(-8, -1);
      }
    }

    showFrame(65);
  }
}

void rippleEffect() {
  for (uint8_t repeat = 0; repeat < 3; ++repeat) {
    for (int radius = 0; radius <= 13; radius += 2) {
      clearFrame();

      for (int y = 0; y < ROWS; ++y) {
        for (int x = 0; x < COLS; ++x) {
          // Coordinates are doubled so the center can lie between LEDs.
          const int dx = abs(2 * x - (COLS - 1));
          const int dy = abs(2 * y - (ROWS - 1));
          const int distance = max(dx, dy);

          if (abs(distance - radius) <= 1) {
            setPixel(x, y);
          }
        }
      }

      showFrame(110);
    }
  }
}

void waveEffect() {
  const uint8_t wave[COLS] = {3, 2, 1, 1, 2, 3, 5, 6, 6, 5, 4, 3};

  for (uint8_t phase = 0; phase < COLS * 5; ++phase) {
    clearFrame();

    for (uint8_t x = 0; x < COLS; ++x) {
      setPixel(x, wave[(x + phase) % COLS]);
    }

    showFrame(75);
  }
}

void bouncingPixelEffect() {
  int x = 1;
  int y = 1;
  int dx = 1;
  int dy = 1;
  int previousX = x;
  int previousY = y;
  int olderX = x;
  int olderY = y;

  for (uint8_t step = 0; step < 70; ++step) {
    clearFrame();
    setPixel(x, y);
    setPixel(previousX, previousY);
    setPixel(olderX, olderY);
    showFrame(55);

    olderX = previousX;
    olderY = previousY;
    previousX = x;
    previousY = y;

    x += dx;
    y += dy;

    if (x <= 0 || x >= COLS - 1) {
      dx = -dx;
      x = constrain(x, 0, COLS - 1);
    }
    if (y <= 0 || y >= ROWS - 1) {
      dy = -dy;
      y = constrain(y, 0, ROWS - 1);
    }
  }
}

void sparkleEffect() {
  for (uint8_t step = 0; step < 45; ++step) {
    clearFrame();

    for (uint8_t sparkle = 0; sparkle < 16; ++sparkle) {
      setPixel(random(COLS), random(ROWS));
    }

    showFrame(80);
  }
}

void heartFinale() {
  uint8_t smallHeart[ROWS][COLS] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0},
    {0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 0},
    {0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0},
    {0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
  };

  uint8_t largeHeart[ROWS][COLS] = {
    {0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0},
    {0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0},
    {1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
    {0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0},
    {0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0}
  };

  for (uint8_t pulse = 0; pulse < 4; ++pulse) {
    matrix.renderBitmap(smallHeart, ROWS, COLS);
    delay(220);
    matrix.renderBitmap(largeHeart, ROWS, COLS);
    delay(320);
  }
}

void setup() {
  matrix.begin();
  randomSeed(analogRead(A0));
  clearFrame();
  showFrame(400);
}

void loop() {
  scannerEffect();
  pauseBetweenEffects();

  rainEffect();
  pauseBetweenEffects();

  rippleEffect();
  pauseBetweenEffects();

  waveEffect();
  pauseBetweenEffects();

  bouncingPixelEffect();
  pauseBetweenEffects();

  sparkleEffect();
  pauseBetweenEffects();

  heartFinale();
  pauseBetweenEffects();
}

