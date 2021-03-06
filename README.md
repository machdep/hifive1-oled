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

### Build under Linux or FreeBSD

Build 32-bit RV32GC toolchain: https://github.com/riscv/riscv-gnu-toolchain

    $ export CROSS_COMPILE=/path/to/riscv32-unknown-elf-
    $ git clone --recursive https://github.com/osfive/hifive1-oled
    $ cd hifive1-oled
    $ make

### Program

Install openocd from https://github.com/riscv/riscv-openocd

    $ sudo openocd -f board/sifive-hifive1.cfg -c "flash protect 0 64 last off; program obj/hifive1-oled.elf verify; resume 0x20400000; exit"

![alt text](https://raw.githubusercontent.com/osfive/hifive1-oled/master/images/hifive1-oled.jpg)
