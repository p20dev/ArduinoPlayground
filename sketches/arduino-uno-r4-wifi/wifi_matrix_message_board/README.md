# Wi-Fi LED Matrix Message Board

Et browserstyret message board til den indbyggede 12×8 LED-matrix på Arduino
UNO R4 WiFi. Websiden kan ændre både besked og scrollhastighed, mens matrixen
kører asynkront.

## Hurtig start uden lokal Wi-Fi-konfiguration

Uden en `arduino_secrets.h` opretter boardet automatisk sit eget netværk:

- Netværk: `Arduino-Matrix`
- Adgangskode: `matrix123`
- Webadresse: `http://192.168.4.1`

Forbind en telefon eller computer til netværket, og åbn adressen i en browser.
Enheden kan advare om, at netværket ikke har internetforbindelse; det er
forventet.

## Brug boardet på dit eksisterende Wi-Fi

1. Kopiér `arduino_secrets.example.h` til `arduino_secrets.h` i samme mappe.
2. Indsæt SSID og adgangskode til et 2.4 GHz-netværk.
3. Kompilér og upload igen.
4. Åbn den IP-adresse, der udskrives ved 115200 baud i Serial Monitor.

`arduino_secrets.h` er ignoreret af Git. Hvis forbindelsen ikke kan oprettes
inden for 15 sekunder, falder sketchen automatisk tilbage til access point-mode.

Matrixfonten er ASCII-baseret. Danske bogstaver konverteres automatisk:
`æ → ae`, `ø → oe` og `å → aa`. Beskeder begrænses til 64 tegn.

## Afhængigheder

- `WiFiS3`, inkluderet i Arduino UNO R4 board-pakken
- `Arduino_LED_Matrix`, inkluderet i board-pakken
- `ArduinoGraphics` 1.1.5 eller nyere

```powershell
arduino-cli lib install ArduinoGraphics
```

## Kompilering

```powershell
arduino-cli compile --fqbn arduino:renesas_uno:unor4wifi sketches/arduino-uno-r4-wifi/wifi_matrix_message_board
```

## Upload

Upload først efter godkendelse:

```powershell
arduino-cli upload --port COM5 --fqbn arduino:renesas_uno:unor4wifi sketches/arduino-uno-r4-wifi/wifi_matrix_message_board
```

