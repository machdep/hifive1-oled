APP =		hifive1-oled
ARCH =		riscv

CC =		${CROSS_COMPILE}gcc
LD =		${CROSS_COMPILE}ld
OBJCOPY =	${CROSS_COMPILE}objcopy

OBJDIR =	obj

LDSCRIPT =	${OBJDIR}/ldscript
LDSCRIPT_TPL =	${CURDIR}/ldscript.tpl
HIFIVE1_FONT =	${CURDIR}/fonts/ter-124n.ld

OBJECTS =	${APP}.o					\
		osfive/sys/riscv/sifive/e300g_aon.o		\
		osfive/sys/riscv/sifive/e300g_prci.o		\
		osfive/sys/riscv/sifive/e300g_spi.o		\
		osfive/sys/riscv/sifive/e300g_clint.o		\
		osfive/sys/riscv/sifive/e300g_gpio.o		\
		osfive/sys/riscv/sifive/e300g_uart.o		\
		osfive/sys/dev/ssd1306/ssd1306.o		\
		osfive/sys/kern/subr_prf.o			\
		osfive/sys/kern/subr_console.o			\
		osfive/sys/kern/kern_panic.o			\
		start.o

LIBRARIES = LIBC LIBFONT

CFLAGS = -g -nostdinc -march=rv32g -mabi=ilp32			\
	-fno-builtin-printf -ffreestanding -Wall		\
	-Wredundant-decls -Wnested-externs			\
	-Wstrict-prototypes -Wmissing-prototypes		\
	-Wpointer-arith -Winline -Wcast-qual			\
	-Wundef -Wmissing-include-dirs -Werror

all:	__compile __link

${LDSCRIPT}:
	sed s#FONT_PATH#${HIFIVE1_FONT}#g ${LDSCRIPT_TPL} > ${LDSCRIPT}

clean:
	rm -f ${OBJECTS} ${LDSCRIPT} ${APP}.elf

include osfive/lib/libc/Makefile.inc
include osfive/lib/libfont/Makefile.inc
include osfive/mk/gnu.mk
