# SOS Blink

Blinker `SOS` (`... --- ...`) i morsekode med den indbyggede LED på D13 på
Arduino UNO R4 Minima. Der kræves ingen eksterne komponenter.

En tidsenhed er 200 ms:

- prik: 1 enhed tændt
- streg: 3 enheder tændt
- mellemrum mellem signaler: 1 enhed
- mellemrum mellem bogstaver: 3 enheder
- mellemrum mellem gentagelser: 7 enheder

## Kompilering

```powershell
arduino-cli compile --fqbn arduino:renesas_uno:minima sketches/arduino-uno-r4-minima/sos_blink
```

## Upload

```powershell
arduino-cli upload --port COM7 --fqbn arduino:renesas_uno:minima sketches/arduino-uno-r4-minima/sos_blink
```

