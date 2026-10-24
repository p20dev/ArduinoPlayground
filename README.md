# Arduino Playground

En samling af Arduino-sketches, eksperimenter og profiler til forskellige
boards.

## Mine boards

### Arduino UNO R4 WiFi

- **Primær MCU:** Renesas RA4M1, 48 MHz Arm Cortex-M4
- **Trådløs MCU:** ESP32-S3-MINI-1-N8
- **Hukommelse:** 256 KB flash, 32 KB SRAM og 8 KB data flash
- **Logikniveau:** 5 V på RA4M1-I/O; Qwiic og ESP32-S3 bruger 3,3 V
- **Trådløst:** Wi-Fi 4 på 2,4 GHz og Bluetooth 5 Low Energy
- **Onboard:** 12×8 rød LED-matrix, RTC, 14-bit ADC, 12-bit DAC på A0,
  OPAMP, USB HID og Qwiic-stik
- **Interfaces:** GPIO, PWM, UART, I²C, SPI, CAN og USB-C
- **Sensorer:** ingen dedikerede miljø- eller bevægelsessensorer; MCU'en har
  intern chiptemperaturmåling og kapacitiv touch-hardware
- **Arduino CLI FQBN:** `arduino:renesas_uno:unor4wifi`

Detaljer, identifikation og links findes i
[`boards/arduino-uno-r4-wifi`](boards/arduino-uno-r4-wifi/README.md).

## Projektstruktur

Boards og sketches bruger samme stabile board-id:

```text
boards/
  arduino-uno-r4-wifi/
    README.md
sketches/
  arduino-uno-r4-wifi/
    led_matrix_demo/
      led_matrix_demo.ino
```

Hver sketch ligger i sin egen mappe, og mappen og den primære `.ino`-fil har
samme navn, så den kan åbnes direkte i Arduino IDE. Board-specifik
dokumentation, forbindelser og biblioteker beskrives ved den enkelte sketch.

## Sketches

### Arduino UNO R4 WiFi

- [`led_matrix_demo`](sketches/arduino-uno-r4-wifi/led_matrix_demo) – animationer
  til den indbyggede LED-matrix
- [`led_matrix_scrolling_text`](sketches/arduino-uno-r4-wifi/led_matrix_scrolling_text)
  – fast løbetekst på matrixen
- [`wifi_matrix_message_board`](sketches/arduino-uno-r4-wifi/wifi_matrix_message_board)
  – browserstyret løbetekst via Wi-Fi

## Tilføjelse af flere boards

Nye boards får en profil under `boards/<board-id>/` og deres sketches placeres
under `sketches/<board-id>/`. Board-id skrives med små bogstaver og bindestreger,
eksempelvis `arduino-uno-r4-wifi`.

