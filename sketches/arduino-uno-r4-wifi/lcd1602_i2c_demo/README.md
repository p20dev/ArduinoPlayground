# LCD1602 I²C-demo

En effekt-demo til et HD44780-kompatibelt 16×2-tegndisplay med den almindelige
PCF8574/PCF8574A I²C-backpack. Demoen viser:

- animeret opstart og blink med baggrundsbelysningen;
- brugerdefinerede hjerte-, smiley- og rakettegn;
- vandret løbetekst;
- en animeret 16-kanals "BYTE BEATS"-equalizer;
- oppetid og spinner.

Sketchen bruger kun `Wire`, som følger med Arduino-kernen. Den scanner selv
PCF8574-adresseområderne `0x20`–`0x27` og `0x38`–`0x3F`, så den typiske adresse
`0x27` behøver ikke angives manuelt.

## Tilslutning

| LCD-backpack | Arduino UNO R4 WiFi |
| --- | --- |
| GND | GND |
| VCC | 5V |
| SDA | SDA |
| SCL | SCL |

Brug de mærkede SDA/SCL-ben ved AREF (eller de tilsvarende A4/A5-signaler).
Hvis baggrundsbelysningen tænder, men tegnene ikke ses, justeres det blå
kontrast-potentiometer forsigtigt med en lille skruetrækker.

## Byg og upload

```powershell
arduino-cli compile --fqbn arduino:renesas_uno:unor4wifi sketches/arduino-uno-r4-wifi/lcd1602_i2c_demo
arduino-cli upload -p COM5 --fqbn arduino:renesas_uno:unor4wifi sketches/arduino-uno-r4-wifi/lcd1602_i2c_demo
```

Serial Monitor kan åbnes ved 115200 baud for at se den fundne I²C-adresse eller
en fejlmeddelelse, hvis displayet ikke findes.
