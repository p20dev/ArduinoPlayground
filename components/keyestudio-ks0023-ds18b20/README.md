# Keyestudio KS0023 DS18B20-temperatursensor

## Identifikation

| Felt | Værdi |
| --- | --- |
| Produkt-id | KS0023 |
| Sensor | Dallas/Maxim DS18B20 |
| Interface | 1-Wire via `DAT`, `VCC`, `GND` |
| Forsyning | 3,3–5 V |
| Måleområde | −55 til +125 °C |

## Tilslutning og hensyn

`DAT` forbindes til en digital pin. Modulet indeholder den nødvendige
pull-up-komponent; flere DS18B20-enheder kan dele samme 1-Wire-bus. Typiske
biblioteker er `OneWire` og `DallasTemperature`. Ingen sketches endnu.

## Kilde

- [Keyestudio KS0023 18B20 Temperature Sensor](https://wiki.keyestudio.com/Ks0023_keyestudio_18B20_Temperature_Sensor)
