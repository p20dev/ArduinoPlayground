// SOS in Morse code on the Arduino UNO R4 Minima built-in LED.
// S = ...  O = ---  S = ...

constexpr unsigned long UNIT_MS = 200;

void blinkSymbol(unsigned long onUnits) {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(onUnits * UNIT_MS);
  digitalWrite(LED_BUILTIN, LOW);

  // Every Morse symbol is followed by a one-unit gap.
  delay(UNIT_MS);
}

void blinkLetter(const char *pattern) {
  for (const char *symbol = pattern; *symbol != '\0'; ++symbol) {
    blinkSymbol(*symbol == '.' ? 1 : 3);
  }
}

void letterGap() {
  // blinkSymbol() already supplied the first of the three gap units.
  delay(2 * UNIT_MS);
}

void wordGap() {
  // blinkSymbol() already supplied the first of the seven gap units.
  delay(6 * UNIT_MS);
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
}

void loop() {
  blinkLetter("...");
  letterGap();

  blinkLetter("---");
  letterGap();

  blinkLetter("...");
  wordGap();
}

