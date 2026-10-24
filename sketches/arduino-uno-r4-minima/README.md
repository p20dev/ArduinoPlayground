# Sketches til Arduino UNO R4 Minima

## Tilgængelige sketches

- [`sos_blink`](sos_blink) – blinker SOS i morsekode med den indbyggede LED

## Struktur

Hver ny sketch skal placeres i sin egen mappe, hvor mappen og `.ino`-filen har
samme navn:

```text
sketches/arduino-uno-r4-minima/
  sos_blink/
    sos_blink.ino
```

Kompilér med:

```powershell
arduino-cli compile --fqbn arduino:renesas_uno:minima sketches/arduino-uno-r4-minima/<sketch-navn>
```

Upload til den aktuelt registrerede port med:

```powershell
arduino-cli upload --port COM7 --fqbn arduino:renesas_uno:minima sketches/arduino-uno-r4-minima/<sketch-navn>
```

