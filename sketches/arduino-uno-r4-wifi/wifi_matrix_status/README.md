# Wi-Fi-status på LED-matrix

Sketchen forbinder Arduino UNO R4 WiFi til et konfigureret hotspot og viser
forbindelsesstatus på boardets indbyggede 12×8 LED-matrix:

- roterende lys: forbindelsen oprettes;
- flueben og 1–3 søjler: forbundet samt aktuel signalstyrke;
- blinkende prik: sketchen kører fortsat;
- kryds: forbindelsen mislykkedes eller blev afbrudt.

Ved udfald forsøger boardet automatisk at forbinde igen. Serial Monitor ved
115200 baud viser hotspotnavn, IP-adresse, signalstyrke og fejlstatus.

## Wi-Fi-konfiguration

Kopiér `arduino_secrets.example.h` til `arduino_secrets.h`, og indsæt hotspot-
oplysningerne. Den rigtige secrets-fil ignoreres af Git og må ikke committes.
UNO R4 WiFi understøtter 2,4 GHz Wi-Fi.

## Byg og upload

```powershell
arduino-cli compile --fqbn arduino:renesas_uno:unor4wifi sketches/arduino-uno-r4-wifi/wifi_matrix_status
arduino-cli upload -p COM5 --fqbn arduino:renesas_uno:unor4wifi sketches/arduino-uno-r4-wifi/wifi_matrix_status
```

## Afhængigheder

`WiFiS3` og `Arduino_LED_Matrix` følger med Arduino UNO R4 board-pakken; der
kræves ingen ekstra biblioteker.
