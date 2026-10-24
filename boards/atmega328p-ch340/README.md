# ATmega328P + CH340 Arduino-kompatibelt board

## Identifikation

| Felt | Verificeret værdi |
| --- | --- |
| Aktuel port | `COM8` |
| USB-serielchip | WCH CH340 |
| USB-ID | `VID 1A86 / PID 7523` |
| Mikrocontroller | ATmega328P |
| MCU-signatur | `1E 95 0F` |
| Bootloaderprotokol | STK500v1/Arduino |
| Uploadhastighed | 115200 baud |
| Anvendt Arduino CLI FQBN | `arduino:avr:uno` |

Identiteten blev kontrolleret skrivebeskyttet med `avrdude`; flashhukommelsen
blev ikke ændret. CH340 og denne bootloaderkombination bruges både på UNO
R3-kloner og nyere Nano-kloner. Den præcise formfaktor kan derfor kun afgøres
ud fra boardets fysiske layout eller silketryk.

## Out-of-the-box

Et standard Arduino-kompatibelt ATmega328P-board tilbyder:

- bruger-LED på D13;
- digitale ind- og udgange;
- seks PWM-kanaler;
- analoge målinger med 10-bit ADC;
- UART, I²C/TWI og SPI;
- eksterne interrupts og pin-change interrupts;
- hardwaretimere, watchdog og 1 KB ikke-flygtig EEPROM.

Boardet har normalt ingen indbyggede sensorer, Wi-Fi eller Bluetooth. Antallet
af analoge pins og strømforsyningsmuligheder afhænger af, om den fysiske model
er UNO-, Nano- eller en anden ATmega328P-kompatibel variant.

## Typiske MCU-data

| Egenskab | Værdi |
| --- | --- |
| Arkitektur | 8-bit AVR |
| Typisk clock | 16 MHz |
| Flash | 32 KB, inklusive bootloader |
| SRAM | 2 KB |
| EEPROM | 1 KB |
| ADC | 10-bit |
| Timere | 2 × 8-bit og 1 × 16-bit |
| Typisk logikniveau | 5 V; kontrollér altid boardets mærkning |

## Sketches i dette repo

- [`sos_blink`](../../sketches/atmega328p-ch340/sos_blink) – blinker SOS i
  morsekode med den standardmæssige indbyggede LED

## Kilder

- [Arduino UNO R3 hardware documentation](https://docs.arduino.cc/hardware/uno-rev3/)
- [Arduino Nano hardware documentation](https://docs.arduino.cc/hardware/nano/)
- [Arduino AVR core board definitions](https://github.com/arduino/ArduinoCore-avr/blob/master/boards.txt)
- [ATmega328P datasheet](https://docs.arduino.cc/resources/datasheets/ATmega328P-datasheet.pdf)
