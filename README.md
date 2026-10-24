# Arduino Playground

En samling af Arduino-sketches til forskellige boards og små hardwareforsøg.

## Struktur

Hver sketch placeres i sin egen mappe under `sketches/`. Mappen og den primære
`.ino`-fil skal have samme navn, så projektet kan åbnes direkte i Arduino IDE.

```text
sketches/
  blink/
    blink.ino
```

Board-specifik dokumentation, forbindelser og nødvendige biblioteker beskrives
i en README ved den enkelte sketch.

## Sketches

- `led_matrix_demo` – animationer til UNO R4 WiFi-matrixen
- `led_matrix_scrolling_text` – fast løbetekst på matrixen
- `wifi_matrix_message_board` – browserstyret løbetekst via Wi-Fi

