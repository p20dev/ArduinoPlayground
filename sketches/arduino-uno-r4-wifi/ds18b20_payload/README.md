# DS18B20 til payload-server

Sketchen måler temperaturen med et Keyestudio KS0023 DS18B20-modul og sender
en JSON-payload til `https://payloads.p20.dev/api/payloads` hvert 10. sekund.
Den indbyggede 12×8 LED-matrix viser samtidig:

- et flueben, når Wi-Fi er forbundet, eller et kryds uden forbindelse;
- en stribe på 10 LED'er, som tæller ned med én LED pr. sekund.

## Tilslutning

| KS0023 | Arduino UNO R4 WiFi |
| --- | --- |
| `DAT` | D2 |
| `VCC` | 5V |
| `GND` | GND |

KS0023-modulet har pull-up-modstanden til 1-Wire-datalinjen monteret på boardet.

## Secrets

Kopiér `arduino_secrets.example.h` til `arduino_secrets.h`. Brug samme
`SECRET_SSID` og `SECRET_PASS` som i `wifi_matrix_status`, og indsæt værdien af
`API_BEARER_TOKEN` fra repoets lokale `.env` som `SECRET_API_TOKEN`.

Den lokale secret-fil er allerede ignoreret af Git.

## Biblioteker og kompilering

Installér `OneWire` og `DallasTemperature` i Arduino IDE's Library Manager.
`WiFiS3` og `Arduino_LED_Matrix` følger med UNO R4-boardpakken.

```powershell
arduino-cli lib install OneWire DallasTemperature
arduino-cli compile --fqbn arduino:renesas_uno:unor4wifi sketches/arduino-uno-r4-wifi/ds18b20_payload
arduino-cli upload -p COM5 --fqbn arduino:renesas_uno:unor4wifi sketches/arduino-uno-r4-wifi/ds18b20_payload
```

API-payloaden har formen:

```json
{
  "topic": "sensors/temperature",
  "payload": {
    "device": "uno-r4-wifi-ds18b20",
    "temperatureC": 22.50
  }
}
```
