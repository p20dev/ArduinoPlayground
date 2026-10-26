# DS18B20 til payload-server

Sketchen måler temperaturen med et Keyestudio KS0023 DS18B20-modul hvert 10.
sekund. Målingerne gemmes i en fast ringbuffer og sendes samlet til
`https://payloads.p20.dev/api/payloads` hvert 5. minut.

Hver måling får sit eget UTC-tidsstempel. Boardet synkroniserer tiden over
Wi-Fi og opdaterer tidsreferencen hver 6. time. Bufferen har plads til 1 times
målinger. Efter et netværksudfald sendes de ældste batches først; kun et
vellykket HTTP 2xx-svar fjerner en batch fra bufferen.

Ved uploadfejl prøver sketchen igen efter et minut. Wi-Fi-forbindelsen nulstilles
efter hver tredje sammenhængende fejl, og boardet genstarter efter den tolvte.
En batch bruger den ældste målings tidsstempel som stabilt `batchId`, så samme
batch kan genkendes, hvis et serversvar går tabt og uploaden gentages.

Et HD44780-kompatibelt LCD1602 med PCF8574 I²C-backpack viser temperatur,
Wi-Fi (`W+`/`W-`), UTC-tid (`U+`/`U-`), heartbeat og antal målinger i bufferen.
Under upload viser det batchstatus og HTTP-status. Sensorfejl, uploadfejl og
automatisk genstart vises som tydelige tekstbeskeder. Sketchen scanner begge
almindelige PCF8574-adresseområder og opdager også et display, der tilsluttes
efter opstart.

## Tilslutning

| KS0023 | Arduino UNO R4 WiFi |
| --- | --- |
| `DAT` | D2 |
| `VCC` | 5V |
| `GND` | GND |

KS0023-modulet har pull-up-modstanden til 1-Wire-datalinjen monteret på boardet.

| LCD1602 I²C | Arduino UNO R4 WiFi |
| --- | --- |
| `GND` | `GND` |
| `VCC` | `5V` |
| `SDA` | `SDA` (eller A4) |
| `SCL` | `SCL` (eller A5) |

Hvis baggrundsbelysningen tænder, men teksten ikke kan ses, justeres det blå
kontrast-potentiometer på I²C-backpacket forsigtigt.

## Secrets

Kopiér `arduino_secrets.example.h` til `arduino_secrets.h`. Brug samme
`SECRET_SSID` og `SECRET_PASS` som i `wifi_matrix_status`, og indsæt værdien af
`API_BEARER_TOKEN` fra repoets lokale `.env` som `SECRET_API_TOKEN`.

Den lokale secret-fil er allerede ignoreret af Git.

## Biblioteker og kompilering

Installér `OneWire` og `DallasTemperature` i Arduino IDE's Library Manager.
`WiFiS3` og `Wire` følger med UNO R4-boardpakken.

```powershell
arduino-cli lib install OneWire DallasTemperature
arduino-cli compile --fqbn arduino:renesas_uno:unor4wifi sketches/arduino-uno-r4-wifi/ds18b20_payload
arduino-cli upload -p COM5 --fqbn arduino:renesas_uno:unor4wifi sketches/arduino-uno-r4-wifi/ds18b20_payload
```

API-payloaden indeholder op til 30 tidsstemplede målinger:

```json
{
  "topic": "sensors/temperature",
  "payload": {
    "device": "uno-r4-wifi-ds18b20",
    "batchId": 1,
    "samples": [
      {
        "measuredAtUnix": 1793025000,
        "temperatureC": 22.50
      }
    ]
  }
}
```
