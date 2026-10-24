# Arduino UNO R4 Minima

## Identifikation

| Felt | Værdi |
| --- | --- |
| Producent | Arduino |
| Model | UNO R4 Minima |
| SKU | ABX00080 |
| Primær MCU | Renesas R7FA4M1AB3CFM (RA4M1) |
| Arduino CLI FQBN | `arduino:renesas_uno:minima` |
| Normal USB-ID | `VID 2341 / PID 0069` |
| Alternativ/bootloader USB-ID | `VID 2341 / PID 0369` |

Boardet blev automatisk identificeret af Arduino CLI på `COM7`. COM-porten kan
ændre sig mellem USB-porte og computere; USB-ID og FQBN er de stabile
identifikatorer.

## Out-of-the-box

Boardet kan bruges uden ekstra komponenter til:

- klassiske Blink- og GPIO-eksperimenter med bruger-LED'en på D13;
- real-time clock og alarmer;
- USB-mus og -tastaturemulering;
- analog måling med op til 14-bit opløsning;
- analog spændings- og bølgeformsgenerering via 12-bit DAC på A0;
- eksperimenter med den indbyggede operationsforstærker;
- seriel kommunikation over USB-C.

RA4M1 tilbyder desuden kapacitiv touch-hardware og CAN-controller. Touch kræver
en ekstern elektrode, og CAN kræver en ekstern transceiver, før funktionerne kan
forbindes fysisk til omverdenen.

Boardet har ingen Wi-Fi, Bluetooth, LED-matrix, Qwiic-stik eller dedikerede
accelerometer-, gyroskop-, lys-, fugt-, tryk- eller rumtemperatursensorer. MCU'ens
interne temperatursensor måler primært chiptemperaturen.

## Elektriske hoveddata

| Egenskab | Værdi |
| --- | --- |
| RA4M1 clock | 48 MHz |
| Flash | 256 KB |
| SRAM | 32 KB |
| Data flash | 8 KB |
| Digitale I/O | 14 |
| Analoge indgange | 6 |
| PWM-udgange | 6 |
| GPIO-logik | 5 V |
| Maks. GPIO-strøm | 8 mA pr. pin |
| Forsyning via USB-C | 5 V |
| VIN / DC-jack | 6–24 V |
| ADC | Op til 14-bit |
| DAC | Op til 12-bit på A0 |
| CAN | Controller indbygget; ekstern transceiver kræves |

## Interfaces og særlige pins

- UART på D0/D1
- I²C på A4/A5 og de dedikerede SDA/SCL-pins
- SPI på D10–D13 og ICSP-headeren
- CAN på D4/D5
- DAC på A0
- OPAMP på A1–A3
- SWD/JTAG-header til debugging

## Sketches i dette repo

Der er endnu ingen Minima-specifikke sketches. Nye sketches placeres under
[`sketches/arduino-uno-r4-minima`](../../sketches/arduino-uno-r4-minima).

## Kilder

- [Arduino UNO R4 Minima hardware documentation](https://docs.arduino.cc/hardware/uno-r4-minima/)
- [Arduino UNO R4 Minima datasheet](https://docs.arduino.cc/resources/datasheets/ABX00080-datasheet.pdf)
- [ArduinoCore-renesas](https://github.com/arduino/ArduinoCore-renesas)
- [Renesas RA4M1 product information](https://www.renesas.com/en/products/ra4m1/part-details/r7fa4m1ab3cfm-aa0)
