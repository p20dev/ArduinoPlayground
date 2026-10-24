# SOS Blink

Blinker `SOS` (`... --- ...`) i morsekode med den standardmæssige indbyggede
LED på D13. Sketchen kræver ingen eksterne komponenter.

En tidsenhed er 200 ms:

- prik: 1 enhed tændt
- streg: 3 enheder tændt
- mellemrum mellem signaler: 1 enhed
- mellemrum mellem bogstaver: 3 enheder
- mellemrum mellem gentagelser: 7 enheder

## Kompilering

```powershell
arduino-cli compile --fqbn arduino:avr:uno sketches/atmega328p-ch340/sos_blink
```

## Upload

Boardet blev registreret på `COM8` med en 115200-baud Optiboot-kompatibel
bootloader:

```powershell
arduino-cli upload --port COM8 --fqbn arduino:avr:uno sketches/atmega328p-ch340/sos_blink
```

