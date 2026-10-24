# Eksterne enheder

Dette katalog registrerer eksterne input- og outputenheder, der bruges sammen
med boards i Arduino Playground: displays, sensorer, aktuatorer, kommunikations-
moduler og andre komponenter.

Hver fysisk enhedstype får sit eget stabile id og en mappe med:

- identifikation og relevante tekniske data;
- pinout, spændingsniveauer og tilslutning;
- eventuelle adresser, biblioteker og kendte faldgruber;
- links til alle sketches i repoet, som bruger enheden.

## Registrerede enheder

| Enhed | Type | Interface | Brugt med | Sketches |
| --- | --- | --- | --- | --- |
| [LCD1602 med PCF8574T](lcd1602-i2c-pcf8574/README.md) | Output/display | I²C | Arduino UNO R4 WiFi | [`lcd1602_i2c_demo`](../sketches/arduino-uno-r4-wifi/lcd1602_i2c_demo) |

## Navngivning

Nye enheder placeres i `devices/<device-id>/`, hvor `device-id` skrives med små
bogstaver og bindestreger, eksempelvis `bme280-temperature-humidity-pressure`.
Profilen opdateres, hver gang enheden bruges i en ny sketch.
