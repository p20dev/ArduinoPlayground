# UNO R3-kompatibelt CH340-board

## Identifikation

| Felt | Verificeret værdi |
| --- | --- |
| Aktuel port | `COM8` |
| Formfaktor | Arduino UNO R3-kompatibel |
| USB-serielchip | WCH CH340 |
| USB-stik | Micro-USB |
| USB-ID | `VID 1A86 / PID 7523` |
| Mikrocontroller | ATmega328P-AU, loddet TQFP-32 |
| MCU-signatur | `1E 95 0F` |
| Bootloaderprotokol | STK500v1/Arduino |
| Uploadhastighed | 115200 baud |
| Anvendt Arduino CLI FQBN | `arduino:avr:uno` |

Identiteten blev kontrolleret skrivebeskyttet med `avrdude`; flashhukommelsen
blev ikke ændret. Fotos af boardet bekræfter UNO R3-formfaktoren, standard
shield-headerlayoutet og ATmega328P-AU-mærkningen.

Boardet er en kompatibel klon, ikke et officielt Arduino UNO R3. De synlige
forskelle omfatter CH340 og Micro-USB i stedet for det officielle boards
ATmega16U2 og USB-B samt en loddet SMD-mikrocontroller i stedet for en udskiftelig
DIP-chip. Det har desuden en ekstra 3×4 breakout-header til RX/TX, I²C og strøm.

## Out-of-the-box

Boardet tilbyder:

- bruger-LED på D13;
- digitale ind- og udgange;
- seks PWM-kanaler;
- analoge målinger med 10-bit ADC;
- UART, I²C/TWI og SPI;
- eksterne interrupts og pin-change interrupts;
- hardwaretimere, watchdog og 1 KB ikke-flygtig EEPROM.

Boardet har ingen indbyggede sensorer, Wi-Fi eller Bluetooth.

## Typiske MCU-data

| Egenskab | Værdi |
| --- | --- |
| Arkitektur | 8-bit AVR |
| Clock | 16 MHz |
| Flash | 32 KB, inklusive bootloader |
| SRAM | 2 KB |
| EEPROM | 1 KB |
| ADC | 10-bit |
| Timere | 2 × 8-bit og 1 × 16-bit |
| Logikniveau | 5 V |
| Digitale I/O | 14 |
| Analoge indgange | 6 |
| PWM-udgange | 6 |

## Sketches i dette repo

- [`sos_blink`](../../sketches/uno-r3-compatible-ch340/sos_blink) – blinker SOS i
  morsekode med den standardmæssige indbyggede LED

## Kilder

- [Arduino UNO R3 hardware documentation](https://docs.arduino.cc/hardware/uno-rev3/)
- [Arduino AVR core board definitions](https://github.com/arduino/ArduinoCore-avr/blob/master/boards.txt)
- [ATmega328P datasheet](https://docs.arduino.cc/resources/datasheets/ATmega328P-datasheet.pdf)
