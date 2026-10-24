# Arduino Mega 2560 Rev3

## Identifikation

| Felt | Verificeret værdi |
| --- | --- |
| Producent | Arduino |
| Model | Mega 2560 Rev3 |
| Aktuel port | `COM9` |
| Primær MCU | ATmega2560 |
| USB-serielcontroller | ATmega16U2 |
| USB-ID | `VID 2341 / PID 0042` |
| Arduino CLI FQBN | `arduino:avr:mega` |

Boardet identificerede sig via USB som `Arduino Mega 2560`, og Arduino CLI
matchede det direkte med den officielle Mega 2560-definition.

## Out-of-the-box

Mega 2560 er især velegnet til projekter med mange forbindelser. Boardet har:

- indbygget bruger-LED på D13;
- 54 digitale ind- og udgange, hvoraf 15 understøtter PWM;
- 16 analoge indgange med 10-bit ADC;
- fire hardware-UARTs (`Serial`, `Serial1`, `Serial2` og `Serial3`);
- I²C/TWI og SPI;
- seks eksterne interrupt-indgange;
- USB-B, DC-jack, ICSP-headere og reset-knap;
- 4 KB ikke-flygtig EEPROM.

Boardet har ingen indbyggede miljø- eller bevægelsessensorer og ingen Wi-Fi
eller Bluetooth.

## Tekniske hoveddata

| Egenskab | Værdi |
| --- | --- |
| Arkitektur | 8-bit AVR |
| Clock | 16 MHz |
| Flash | 256 KB, hvoraf 8 KB bruges af bootloaderen |
| SRAM | 8 KB |
| EEPROM | 4 KB |
| Logikniveau | 5 V |
| Digitale I/O | 54 |
| PWM-udgange | 15 |
| Analoge indgange | 16 |
| Hardware-UARTs | 4 |

## Sketches i dette repo

- [`sos_blink`](../../sketches/arduino-mega-2560/sos_blink) – blinker SOS i
  morsekode med den indbyggede D13-LED

## Kilder

- [Arduino Mega 2560 Rev3 hardware documentation](https://docs.arduino.cc/hardware/mega-2560/)
- [Arduino Mega 2560 Rev3 datasheet](https://docs.arduino.cc/resources/datasheets/A000067-datasheet.pdf)
- [Arduino AVR core board definitions](https://github.com/arduino/ArduinoCore-avr/blob/master/boards.txt)
