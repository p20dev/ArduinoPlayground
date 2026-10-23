# LED Matrix Demo

En selvkørende demo til den indbyggede 12×8 LED-matrix på Arduino UNO R4 WiFi.

Demoen gentager følgende effekter:

1. Scanner frem og tilbage
2. Digital regn
3. Udvidende ringe
4. Bevægelig bølge
5. Hoppende pixel med hale
6. Tilfældige glimt
7. Pulserende hjerte

Sketchen bruger `Arduino_LED_Matrix`, som følger med Arduino Renesas UNO-kernen.
Der kræves ingen ekstra komponenter eller biblioteker.

## Kompilering

```powershell
arduino-cli compile --fqbn arduino:renesas_uno:unor4wifi sketches/led_matrix_demo
```

## Upload

Upload først efter godkendelse:

```powershell
arduino-cli upload --port COM5 --fqbn arduino:renesas_uno:unor4wifi sketches/led_matrix_demo
```

