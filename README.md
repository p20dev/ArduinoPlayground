# Arduino Playground

En samling af Arduino-sketches, eksperimenter og profiler til forskellige
boards.

Repoet indeholder desuden en Docker Compose-baseret server til REST-payloads
og et aktivitetsdashboard. Se [`server/`](server/README.md).

Repoet registrerer både Arduino-boards og de eksterne input/output-enheder, der
tilsluttes dem. Boardprofiler ligger i `boards/`, enhedsprofiler i `devices/`,
og hver enhedsprofil linker til de sketches, hvor enheden bruges.

## Boards

| Board | Primær MCU | Flash / SRAM | I/O | Trådløst og særlige onboard-funktioner | Arduino CLI FQBN |
| --- | --- | --- | --- | --- | --- |
| [Arduino Mega 2560 Rev3](boards/arduino-mega-2560/README.md) | ATmega2560, 16 MHz AVR | 256 KB / 8 KB | 54 digitale, 15 PWM, 16 analoge, 4 UART | Ingen trådløst; D13-LED | `arduino:avr:mega` |
| [Arduino UNO R4 WiFi](boards/arduino-uno-r4-wifi/README.md) | Renesas RA4M1, 48 MHz Cortex-M4 | 256 KB / 32 KB | GPIO, PWM, ADC, DAC, UART, I²C, SPI og CAN | Wi-Fi, BLE, 12×8 LED-matrix, RTC og Qwiic | `arduino:renesas_uno:unor4wifi` |
| [Arduino UNO R4 Minima](boards/arduino-uno-r4-minima/README.md) | Renesas RA4M1, 48 MHz Cortex-M4 | 256 KB / 32 KB | GPIO, PWM, ADC, DAC, UART, I²C, SPI og CAN | Ingen trådløst; D13-LED, RTC og USB HID | `arduino:renesas_uno:minima` |
| [UNO R3-kompatibelt CH340-board](boards/uno-r3-compatible-ch340/README.md) | ATmega328P-AU, 16 MHz AVR | 32 KB / 2 KB | 14 digitale, 6 PWM og 6 analoge | Ingen trådløst; D13-LED og CH340 USB-seriel | `arduino:avr:uno` |

### Arduino Mega 2560 Rev3

- **Primær MCU:** ATmega2560, 16 MHz 8-bit AVR
- **USB-seriel:** ATmega16U2, observeret som `VID 2341 / PID 0042`
- **Hukommelse:** 256 KB flash, 8 KB SRAM og 4 KB EEPROM
- **Logikniveau:** 5 V
- **I/O:** 54 digitale ben, 15 PWM-udgange og 16 analoge indgange
- **Interfaces:** fire UARTs, I²C, SPI, USB-B, ICSP og eksterne interrupts
- **Onboard:** bruger-LED på D13, reset-knap og DC-jack
- **Trådløst og sensorer:** ingen indbyggede
- **Arduino CLI FQBN:** `arduino:avr:mega`

Boardet blev identificeret direkte via USB og Arduino CLI. Detaljer og kilder
findes i [`boards/arduino-mega-2560`](boards/arduino-mega-2560/README.md).

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

### Arduino UNO R4 Minima

- **Primær MCU:** Renesas RA4M1, 48 MHz Arm Cortex-M4
- **Hukommelse:** 256 KB flash, 32 KB SRAM og 8 KB data flash
- **Logikniveau:** 5 V
- **Onboard:** bruger-LED på D13, RTC, 14-bit ADC, 12-bit DAC på A0, OPAMP,
  USB HID og reset-knap
- **Interfaces:** GPIO, PWM, UART, I²C, SPI, CAN, SWD/JTAG og USB-C
- **Trådløst:** ingen Wi-Fi eller Bluetooth
- **Sensorer:** ingen dedikerede miljø- eller bevægelsessensorer; MCU'en har
  intern chiptemperaturmåling og kapacitiv touch-hardware
- **Arduino CLI FQBN:** `arduino:renesas_uno:minima`

Minima har ikke LED-matrix eller Qwiic-stik. CAN kræver en ekstern transceiver.
Detaljer, identifikation og links findes i
[`boards/arduino-uno-r4-minima`](boards/arduino-uno-r4-minima/README.md).

### UNO R3-kompatibelt CH340-board

- **Formfaktor:** Arduino UNO R3-kompatibelt shield-layout
- **MCU:** ATmega328P-AU i loddet TQFP-kapsling, signatur `1E 95 0F`
- **USB-seriel:** WCH CH340, `VID 1A86 / PID 7523`
- **USB-stik:** Micro-USB
- **Bootloader:** STK500v1/Optiboot-kompatibel ved 115200 baud
- **Hukommelse:** 32 KB flash, 2 KB SRAM og 1 KB EEPROM
- **Interfaces:** 14 digitale I/O, 6 PWM, 6 analoge indgange, UART, I²C og SPI
- **Onboard:** bruger-LED på D13, reset-knap, ICSP, DC-jack og ekstra breakout-header
- **Trådløst og sensorer:** ingen indbyggede
- **Anvendt FQBN:** `arduino:avr:uno`

Billederne bekræfter UNO R3-formfaktoren. Det er et kompatibelt klon-board og
ikke et officielt UNO R3, som bruger USB-B og ATmega16U2 til USB. Detaljer findes
i [`boards/uno-r3-compatible-ch340`](boards/uno-r3-compatible-ch340/README.md).

## Projektstruktur

Boards og sketches bruger samme stabile board-id. Eksterne enheder har deres
eget stabile device-id:

```text
boards/
  arduino-mega-2560/
    README.md
  arduino-uno-r4-minima/
    README.md
  arduino-uno-r4-wifi/
    README.md
devices/
  lcd1602-i2c-pcf8574/
    README.md
sketches/
  arduino-mega-2560/
    sos_blink/
      sos_blink.ino
  arduino-uno-r4-minima/
    README.md
  arduino-uno-r4-wifi/
    led_matrix_demo/
      led_matrix_demo.ino
```

Hver sketch ligger i sin egen mappe, og mappen og den primære `.ino`-fil har
samme navn, så den kan åbnes direkte i Arduino IDE. Board-specifik
dokumentation, forbindelser og biblioteker beskrives ved den enkelte sketch.

## Eksterne input/output-enheder

### LCD1602 med PCF8574T I²C-backpack

- **Type:** 16×2 HD44780-kompatibelt tegndisplay
- **Retning:** output
- **Interface:** I²C via PCF8574T
- **Forsyning i projektet:** 5 V
- **Brugt med:** Arduino UNO R4 WiFi
- **Sketch:** [`lcd1602_i2c_demo`](sketches/arduino-uno-r4-wifi/lcd1602_i2c_demo)

Pinout, adresseoplysninger, praktiske noter og den samlede sketch-liste findes i
[`devices/lcd1602-i2c-pcf8574`](devices/lcd1602-i2c-pcf8574/README.md). Det
samlede enhedsindeks findes i [`devices/`](devices/README.md).

## Sketches

### Arduino Mega 2560 Rev3

- [`sos_blink`](sketches/arduino-mega-2560/sos_blink) – SOS i morsekode på den
  indbyggede D13-LED

### Arduino UNO R4 WiFi

- [`led_matrix_demo`](sketches/arduino-uno-r4-wifi/led_matrix_demo) – animationer
  til den indbyggede LED-matrix
- [`led_matrix_scrolling_text`](sketches/arduino-uno-r4-wifi/led_matrix_scrolling_text)
  – fast løbetekst på matrixen
- [`wifi_matrix_message_board`](sketches/arduino-uno-r4-wifi/wifi_matrix_message_board)
  – browserstyret løbetekst via Wi-Fi
- [`wifi_matrix_status`](sketches/arduino-uno-r4-wifi/wifi_matrix_status) – forbinder
  til et hotspot og viser forbindelse samt signalstyrke på LED-matrixen
- [`lcd1602_i2c_demo`](sketches/arduino-uno-r4-wifi/lcd1602_i2c_demo) – effekt-demo
  til et eksternt 16×2 HD44780-display med PCF8574 I²C-backpack

### Arduino UNO R4 Minima

- [`sos_blink`](sketches/arduino-uno-r4-minima/sos_blink) – SOS i morsekode på
  den indbyggede LED

### UNO R3-kompatibelt CH340-board

- [`sos_blink`](sketches/uno-r3-compatible-ch340/sos_blink) – SOS i morsekode på den
  standardmæssige D13-LED

## Tilføjelse af flere boards og enheder

Nye boards får en profil under `boards/<board-id>/` og deres sketches placeres
under `sketches/<board-id>/`. Board-id skrives med små bogstaver og bindestreger,
eksempelvis `arduino-uno-r4-wifi`.

Nye eksterne enheder får en profil under `devices/<device-id>/`. Profilen skal
beskrive identifikation, pinout og elektriske hensyn samt linke til alle
sketches, der bruger enheden. Både dette README og `devices/README.md` holdes
opdateret, når nye enheder tilføjes.

