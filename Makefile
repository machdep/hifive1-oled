APP =		hifive1-oled
ARCH =		riscv

CC =		${CROSS_COMPILE}gcc
LD =		${CROSS_COMPILE}ld
OBJCOPY =	${CROSS_COMPILE}objcopy

LDSCRIPT =	${.OBJDIR}/ldscript
LDSCRIPT_TPL =	${.CURDIR}/ldscript.tpl
HIFIVE1_FONT =	${.CURDIR}/fonts/ter-124n.ld

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
		osfive/lib/libfont/libfont.o			\
		osfive/lib/libc/stdio/printf.o			\
		osfive/lib/libc/string/strlen.o			\
		osfive/lib/libc/string/bzero.o			\
		start.o

CFLAGS =	-O -pipe -g -nostdinc -fno-omit-frame-pointer		\
	-march=rv32g -mabi=ilp32 -fno-builtin-printf			\
	-fno-optimize-sibling-calls -ffreestanding -fwrapv		\
	-fdiagnostics-show-option -fms-extensions -finline-limit=8000	\
	-Wall -Wredundant-decls -Wnested-externs -Wstrict-prototypes	\
	-Wmissing-prototypes -Wpointer-arith -Winline -Wcast-qual	\
	-Wundef -Wno-pointer-sign -Wno-format -Wmissing-include-dirs	\
	-Wno-unknown-pragmas -Werror

all:	compile link

${LDSCRIPT}:
	sed s#FONT_PATH#${HIFIVE1_FONT}#g ${LDSCRIPT_TPL} > ${LDSCRIPT}

clean:
	rm -f ${OBJECTS:M*} ${LDSCRIPT} ${APP}.elf

.include "osfive/mk/user.mk"
.include "osfive/mk/compile.mk"
.include "osfive/mk/link.mk"
