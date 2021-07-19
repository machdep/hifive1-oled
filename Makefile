APP =		hifive1-oled
MACHINE =	riscv

CC =		${CROSS_COMPILE}gcc
LD =		${CROSS_COMPILE}ld
OBJCOPY =	${CROSS_COMPILE}objcopy

OBJDIR =	obj

LDSCRIPT =	${OBJDIR}/ldscript
LDSCRIPT_TPL =	${CURDIR}/src/ldscript.tpl
HIFIVE1_FONT =	${CURDIR}/fonts/ter-124n.ld

export CFLAGS = -g -nostdinc -march=rv32ima -mabi=ilp32		\
	-fno-builtin-printf -ffreestanding -Wall		\
	-Wredundant-decls -Wnested-externs			\
	-Wstrict-prototypes -Wmissing-prototypes		\
	-Wpointer-arith -Winline -Wcast-qual			\
	-Wundef -Wmissing-include-dirs -Werror

export AFLAGS = ${CFLAGS}

all:	${LDSCRIPT}
	@python3 -B mdepx/tools/emitter.py mdepx.conf

${LDSCRIPT}:
	@sed s#FONT_PATH#${HIFIVE1_FONT}#g ${LDSCRIPT_TPL} > ${LDSCRIPT}

clean:
	@rm -f ${OBJECTS} ${LDSCRIPT} ${OBJDIR}/${APP}.elf

include mdepx/mk/user.mk
