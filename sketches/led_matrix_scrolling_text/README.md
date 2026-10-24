# LED Matrix Scrolling Text

Løbetekst til den indbyggede 12×8 LED-matrix på Arduino UNO R4 WiFi.

Teksten ruller fra højre mod venstre og gentages efter en kort pause:

> Kan AI skrive programmer til Arduino? Tjek https://iot.p20.dev

Sketchen bruger de officielle biblioteker `ArduinoGraphics` og
`Arduino_LED_Matrix`. `ArduinoGraphics` kan installeres med:

```powershell
arduino-cli lib install ArduinoGraphics
```

## Kompilering

```powershell
arduino-cli compile --fqbn arduino:renesas_uno:unor4wifi sketches/led_matrix_scrolling_text
```

## Upload

Upload først efter godkendelse:

```powershell
arduino-cli upload --port COM5 --fqbn arduino:renesas_uno:unor4wifi sketches/led_matrix_scrolling_text
```

