# Sketches til Arduino UNO R4 Minima

Der er endnu ingen Minima-specifikke sketches i projektet.

Hver ny sketch skal placeres i sin egen mappe, hvor mappen og `.ino`-filen har
samme navn:

```text
sketches/arduino-uno-r4-minima/
  blink/
    blink.ino
```

Kompilér med:

```powershell
arduino-cli compile --fqbn arduino:renesas_uno:minima sketches/arduino-uno-r4-minima/<sketch-navn>
```

Upload til den aktuelt registrerede port med:

```powershell
arduino-cli upload --port COM7 --fqbn arduino:renesas_uno:minima sketches/arduino-uno-r4-minima/<sketch-navn>
```

