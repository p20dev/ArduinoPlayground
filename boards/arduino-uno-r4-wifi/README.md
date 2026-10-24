# Arduino UNO R4 WiFi

## Identifikation

| Felt | Værdi |
| --- | --- |
| Producent | Arduino |
| Model | UNO R4 WiFi |
| Primær MCU | Renesas R7FA4M1AB3CFM (RA4M1) |
| Trådløs MCU | ESP32-S3-MINI-1-N8 |
| Arduino CLI FQBN | `arduino:renesas_uno:unor4wifi` |
| Normal USB-ID | `VID 2341 / PID 1002` |
| USB-bootloader-ID | `VID 2341 / PID 006D` |

## Out-of-the-box

Boardet kan bruges uden ekstra komponenter til:

- grafik, animationer og løbetekst på den indbyggede 12×8 LED-matrix;
- Wi-Fi-klient, access point, webserver, HTTPS, MQTT, UDP og NTP;
- Bluetooth Low Energy som peripheral eller central;
- real-time clock og alarmer;
- USB-mus og -tastaturemulering;
- analog måling via 14-bit ADC;
- analog spændingsgenerering via 12-bit DAC på A0;
- signalbehandling med den indbyggede operationsforstærker;
- CAN-kommunikation med en ekstern CAN-transceiver.

Boardet har ingen dedikeret accelerometer-, gyroskop-, lys-, fugt-, tryk- eller
rumtemperatursensor. RA4M1 har en intern temperatursensor til chiptemperatur og
kapacitiv touch-hardware, men ingen færdige touchflader på selve boardet.

## Elektriske hoveddata

| Egenskab | Værdi |
| --- | --- |
| RA4M1 clock | 48 MHz |
| Flash | 256 KB |
| SRAM | 32 KB |
| Data flash | 8 KB |
| GPIO-logik | 5 V |
| Forsyning via USB-C | 5 V |
| VIN / DC-jack | 6–24 V |
| Wi-Fi | 802.11 b/g/n, 2,4 GHz |
| Bluetooth | Bluetooth 5 Low Energy |
| Matrix | 12×8 røde LEDs |
| Qwiic | 3,3 V I²C |

## Sketches i dette repo

- [`led_matrix_demo`](../../sketches/arduino-uno-r4-wifi/led_matrix_demo)
- [`led_matrix_scrolling_text`](../../sketches/arduino-uno-r4-wifi/led_matrix_scrolling_text)
- [`wifi_matrix_message_board`](../../sketches/arduino-uno-r4-wifi/wifi_matrix_message_board)
- [`wifi_matrix_status`](../../sketches/arduino-uno-r4-wifi/wifi_matrix_status)
- [`lcd1602_i2c_demo`](../../sketches/arduino-uno-r4-wifi/lcd1602_i2c_demo)

## Kilder

- [Arduino UNO R4 WiFi hardware documentation](https://docs.arduino.cc/hardware/uno-r4-wifi/)
- [Arduino UNO R4 WiFi datasheet](https://docs.arduino.cc/resources/datasheets/ABX00087-datasheet.pdf)
- [Arduino UNO R4 WiFi network examples](https://docs.arduino.cc/tutorials/uno-r4-wifi/wifi-examples)
- [ArduinoCore-renesas](https://github.com/arduino/ArduinoCore-renesas)
- [Renesas RA4M1 product information](https://www.renesas.com/en/products/ra4m1/part-details/r7fa4m1ab3cfm-aa0)
