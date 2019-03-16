/*-
 * Copyright (c) 2018 Ruslan Bukin <br@bsdpad.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <sys/cdefs.h>
#include <sys/console.h>
#include <sys/types.h>
#include <sys/systm.h>
#include <sys/thread.h>
#include <sys/malloc.h>

#include <machine/machdep.h>
#include <machine/cpufunc.h>
#include <machine/cpuregs.h>

#include <dev/spi/spi.h>
#include <dev/ssd1306/ssd1306.h>
#include <riscv/sifive/e300g.h>

#include <libfont/libfont.h>

static struct clint_softc clint_sc;
static struct aon_softc aon_sc;
static struct spi_softc spi_sc;
static struct prci_softc prci_sc;
static struct gpio_softc gpio_sc;
static struct uart_softc uart_sc;

spi_device_t spi_dev;

extern uint32_t _smem;
extern uint32_t _sdata;
extern uint32_t _edata;
extern uint32_t _sbss;
extern uint32_t _ebss;
extern uint32_t _sfont;

#define	BOARD_OSC_FREQ	32768
#define	SPI_CS		0

/* SSD1306 pins */
#define	PIN_DC		22	/* Data or command */
#define	PIN_RESET	23	/* Active low */

static struct global_data {
	uint8_t buffer[128 * 64 / 8];
	uint8_t *ptr;
	struct font_info font;
} g_data;

/*
 * Data or command:
 * 0 - data
 * 1 - command
 */
static void
oled_dc(struct gpio_softc *sc, uint8_t value)
{

	e300g_gpio_port(&gpio_sc, PIN_DC, value);
}

static void
oled_cmd(spi_device_t *dev, uint8_t cmd)
{

	dev->transfer(dev, &cmd, NULL, 1);
}

void
udelay(uint32_t usec)
{

	clint_udelay(&clint_sc, usec, BOARD_OSC_FREQ);
}

static void
uart_putchar(int c, void *arg)
{
	struct uart_softc *sc;

	sc = arg;

	if (sc == NULL)
		return;

	if (c == '\n')
		e300g_uart_putc(sc, '\r');

	e300g_uart_putc(sc, c);
}

static void
draw_pixel(void *arg, int x, int y, int pixel)
{
	struct global_data *gd;

	gd = arg;

	ssd1306_draw_pixel(gd->ptr, x, y, pixel);
}

static void
draw_text(char *z)
{
	struct char_info ci;
	int i;

	g_data.ptr = (uint8_t *)&g_data.buffer[0];
	for (i = 0; i < strlen(z); i++) {
		get_char_info(&g_data.font, z[i], &ci);
		draw_char(&g_data.font, z[i]);
		g_data.ptr += ci.xsize;
	}
}

static void
e300g_init(void)
{
	uint32_t cpu_freq;

	e300g_clint_init(&clint_sc, CLINT_BASE);
	e300g_aon_init(&aon_sc, AON_BASE);
	e300g_prci_init(&prci_sc, PRCI_BASE);

	e300g_gpio_init(&gpio_sc, GPIO_BASE);
	e300g_iof0_enable(&gpio_sc, IOF0_UART0_RX | IOF0_UART0_TX);
	e300g_iof0_enable(&gpio_sc, IOF0_SPI1_MOSI | IOF0_SPI1_SS0
	    | IOF0_SPI1_SCK);

	e300g_spi_init(&spi_sc, &spi_dev, SPI1_BASE, SPI_CS);

	cpu_freq = clint_get_cpu_freq(&clint_sc, BOARD_OSC_FREQ);
	e300g_uart_init(&uart_sc, UART0_BASE, cpu_freq, 115200);
}

static void
oled_init(void)
{

	e300g_gpio_output_enable(&gpio_sc, PIN_DC, 1);
	e300g_gpio_output_enable(&gpio_sc, PIN_RESET, 1);

	/* Reset display */
	e300g_gpio_port(&gpio_sc, PIN_RESET, 0);
	udelay(10000);
	e300g_gpio_port(&gpio_sc, PIN_RESET, 1);
	udelay(10000);

	/* Init SSD1306 */
	oled_dc(&gpio_sc, 0);
	ssd1306_init(&spi_dev);
}

static void
clear_display(void)
{
	int i;

	for (i = 0; i < 128*64/8; i++)
		g_data.buffer[i] = 0;
}

static void
clear_bss(void)
{
	uint8_t *sbss;
	uint8_t *ebss;

	sbss = (uint8_t *)&_sbss;
	ebss = (uint8_t *)&_ebss;

	while (sbss < ebss)
		*sbss++ = 0;
}

static void
copy_to_ram(void)
{
	uint8_t *dst;
	uint8_t *src;

	/* Copy sdata to RAM if required */
	for (src = (uint8_t *)&_smem, dst = (uint8_t *)&_sdata;
	    dst < (uint8_t *)&_edata; )
		*dst++ = *src++;
}

void
main(void)
{
	char text[16];
	uint16_t c;
	int i;
	int g;

	clear_bss();
	copy_to_ram();
	md_init();

	e300g_init();

	console_register(uart_putchar, (void *)&uart_sc);

	bzero(&g_data.font, sizeof(struct font_info));
	font_init(&g_data.font, (uint8_t *)&_sfont);
	g_data.font.draw_pixel = draw_pixel;
	g_data.font.draw_pixel_arg = &g_data;

	malloc_init();
	malloc_add_region(0x80003000, 0x1000);

	printf("Hello World!\n");

	oled_init();

	c = 0;
	while (1) {
		clear_display();
		sprintf(text, "osfive %d", c);
		draw_text(text);

		oled_dc(&gpio_sc, 0);
		oled_cmd(&spi_dev, SSD1306_COLUMNADDR);
		oled_cmd(&spi_dev, 0);
		oled_cmd(&spi_dev, 127);

		oled_cmd(&spi_dev, SSD1306_PAGEADDR);
		oled_cmd(&spi_dev, 0);
		oled_cmd(&spi_dev, 7);

		/* Ensure all entries gone. */
		e300g_spi_poll_txwm(&spi_dev);

		for (g = 0; g < 32; g++)
			__asm("nop");

		oled_dc(&gpio_sc, 1);
		for (i = 0; i < (128 * 64 / 8); i++)
			oled_cmd(&spi_dev, g_data.buffer[i]);

		if (++c > 999)
			c = 0;
	}
}
