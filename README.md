# hifive1-oled

Display used in this demo: 0.96" 128x64 SPI SSD1306 OLED

OLED display is available here:
https://www.aliexpress.com/wholesale?SearchText=128x64+SPI+SSD1306

Connect OLED display to HiFive1 board using this table:

| SSD1306                 | HiFive1        |
| ----------------------- | -------------- |
| GND                     | GND            |
| VCC                     | 3.3V           |
| D0 (CLK)                | 13 (SPI1 SCK)  |
| D1 (MISO)               | 11 (SPI1 MOSI) |
| RES (Reset)             | 7              |
| DC (Data or Command)    | 6              |
| CS (Chip Select)        | 10             |
