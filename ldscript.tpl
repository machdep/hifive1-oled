OUTPUT_ARCH("riscv")

ENTRY(_start)

MEMORY
{
	flash (rxai!w) : ORIGIN = 0x20400000, LENGTH = 16M
	sram (wxa!ri)  : ORIGIN = 0x80000000, LENGTH = 16K
}

SECTIONS
{
	. = 0x20400000;

	.start . : {
		*start.o(.text)
	} > flash

	.text : {
		*(.text)
	} > flash

	.rodata : {
		*(.rodata)

		. = ALIGN(4);
		_sfont = ABSOLUTE(.);
		INCLUDE "FONT_PATH";
		_efont = ABSOLUTE(.);
	} > flash

	.sdata : {
		*(.sdata)
	} > sram AT > flash

	.bss : {
		*(.bss COMMON)
	} > sram

	. = ALIGN(4);

	stack_top = . + 0x1000; /* 4kB of stack memory */
}
