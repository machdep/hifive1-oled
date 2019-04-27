APP =		hifive1-oled
MACHINE =	riscv

CC =		${CROSS_COMPILE}gcc
LD =		${CROSS_COMPILE}ld
OBJCOPY =	${CROSS_COMPILE}objcopy

OBJDIR =	obj

LDSCRIPT =	${OBJDIR}/ldscript
LDSCRIPT_TPL =	${CURDIR}/ldscript.tpl
HIFIVE1_FONT =	${CURDIR}/fonts/ter-124n.ld

OBJECTS =	main.o						\
		osfive/sys/riscv/sifive/e300g_aon.o		\
		osfive/sys/riscv/sifive/e300g_prci.o		\
		osfive/sys/riscv/sifive/e300g_spi.o		\
		osfive/sys/riscv/sifive/e300g_clint.o		\
		osfive/sys/riscv/sifive/e300g_gpio.o		\
		osfive/sys/riscv/sifive/e300g_uart.o		\
		osfive/sys/dev/ssd1306/ssd1306.o		\
		start.o

KERNEL = malloc
LIBRARIES = libc libfont

CFLAGS = -g -nostdinc -march=rv32im -mabi=ilp32			\
	-fno-builtin-printf -ffreestanding -Wall		\
	-Wredundant-decls -Wnested-externs			\
	-Wstrict-prototypes -Wmissing-prototypes		\
	-Wpointer-arith -Winline -Wcast-qual			\
	-Wundef -Wmissing-include-dirs -Werror

all:	${OBJDIR}/${APP}.elf

${LDSCRIPT}:
	@sed s#FONT_PATH#${HIFIVE1_FONT}#g ${LDSCRIPT_TPL} > ${LDSCRIPT}

clean:
	@rm -f ${OBJECTS} ${LDSCRIPT} ${OBJDIR}/${APP}.elf

include osfive/lib/libc/Makefile.inc
include osfive/lib/libfont/Makefile.inc
include osfive/mk/default.mk
