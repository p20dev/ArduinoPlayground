# LCD1602 med PCF8574T I²C-backpack

## Identifikation

| Felt | Værdi |
| --- | --- |
| Enhedstype | 16×2 monokromt tegndisplay (output) |
| Display-controller | HD44780-kompatibel |
| I²C-ekspander | NXP PCF8574T |
| Synlige tegn | 16 kolonner × 2 linjer |
| Tegnmatrix | 5×8 pixels pr. tegn |
| Brugerdefinerede tegn | 8 samtidige CGRAM-tegn |
| Backpack-ben | GND, VCC, SDA, SCL |
| Typisk I²C-adresse | `0x27` med ubrudte A0/A1/A2-loddebroer |
| Forsyning i projektet | 5 V |

Enheden er identificeret fra billederne som et klassisk LCD1602-modul med en
påmonteret PCF8574T-backpack. Backpacken omsætter I²C til displayets parallelle
HD44780-interface og har et blåt potentiometer til kontrastjustering.

## Pinout og tilslutning

| LCD-backpack | Funktion | Arduino UNO R4 WiFi |
| --- | --- | --- |
| GND | Stel | GND |
| VCC | Forsyning og baggrundslys | 5V |
| SDA | I²C-data | SDA |
| SCL | I²C-clock | SCL |

På UNO R4 WiFi kan de mærkede SDA/SCL-ben ved AREF bruges. De svarer til
boardets primære I²C-bus.

## Adresse og backpack-layout

PCF8574T bruger normalt adresseområdet `0x20`–`0x27`; varianten PCF8574AT
bruger `0x38`–`0x3F`. A0/A1/A2-loddebroerne bestemmer den konkrete adresse.
Den anvendte demo scanner begge områder automatisk.

Det almindelige backpack-layout er:

| PCF8574-bit | LCD-funktion |
| --- | --- |
| P0 | RS |
| P1 | R/W |
| P2 | Enable |
| P3 | Baggrundsbelysning |
| P4–P7 | D4–D7 |

## Praktiske noter

- Hvis baggrundslyset virker, men der ikke ses tegn, justeres det blå
  kontrast-potentiometer langsomt med en lille skruetrækker.
- Displayet skal have fælles GND med Arduino-boardet.
- Fjern strømmen, før ledninger flyttes.
- Sketchen bruger kun Arduino-kernens `Wire`-bibliotek; backpack-driveren er
  implementeret direkte i sketchen.

## Bruges i disse sketches

| Board | Sketch | Formål |
| --- | --- | --- |
| Arduino UNO R4 WiFi | [`lcd1602_i2c_demo`](../../sketches/arduino-uno-r4-wifi/lcd1602_i2c_demo) | Tekstkort, specialtegn, equalizer, PIXEL PONG, PIXEL RAIN og oppetid |

Når enheden bruges i flere sketches, tilføjes de til tabellen ovenfor.
