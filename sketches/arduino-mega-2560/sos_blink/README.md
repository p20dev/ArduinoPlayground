# SOS-blink til Arduino Mega 2560

Sketchen blinker `SOS` (`... --- ...`) i morsekode på boardets indbyggede
LED, som er forbundet til D13.

Timing med `UNIT_MS = 200`:

- prik: 1 enhed;
- streg: 3 enheder;
- pause mellem symboler: 1 enhed;
- pause mellem bogstaver: 3 enheder;
- pause mellem gentagelser: 7 enheder.

## Byg og upload

```powershell
arduino-cli compile --fqbn arduino:avr:mega sketches/arduino-mega-2560/sos_blink
arduino-cli upload -p COM9 --fqbn arduino:avr:mega sketches/arduino-mega-2560/sos_blink
```
