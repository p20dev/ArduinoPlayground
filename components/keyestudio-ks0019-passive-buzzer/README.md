# Keyestudio KS0019 passiv buzzer

## Identifikation

| Felt | Værdi |
| --- | --- |
| Produkt-id | KS0019 |
| Type | Passiv piezo-buzzer uden intern oscillator |
| Interface | `IN`, `VCC`, `GND` |
| Forsyning | 3,3–5 V |

Den åbne lydgiver kræver et firkantsignal og kan derfor afspille forskellige
frekvenser. Den skal ikke forveksles med KS0018 aktiv buzzer.

## Tilslutning

`IN` forbindes til en digital udgang og styres eksempelvis med `tone()`.
Kontinuerlig DC giver ikke en kontrolleret tone. Ingen sketches endnu.

## Kilde

- [Keyestudio KS0019 Passive Buzzer](https://wiki.keyestudio.com/Ks0019_keyestudio_Passive_Buzzer_module)
