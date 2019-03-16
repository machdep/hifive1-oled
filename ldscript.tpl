OUTPUT_ARCH("riscv")

ENTRY(_start)

MEMORY
{
	flash (rxai!w) : ORIGIN = 0x20400000, LENGTH = 16M
	sram (wxa!ri)  : ORIGIN = 0x80000000, LENGTH = 12K
	sram1 (wxa!ri) : ORIGIN = 0x80003000, LENGTH = 4K /* malloc */
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

	_smem = ABSOLUTE(.);
	.sdata : {
		_sdata = ABSOLUTE(.);
		*(.sdata)
		_edata = ABSOLUTE(.);
	} > sram AT > flash

	.bss : {
		_sbss = ABSOLUTE(.);
		*(.bss COMMON)
		*(.sbss)
		_ebss = ABSOLUTE(.);
	} > sram

	. = ALIGN(4);

	stack_top = . + 0x1000; /* 4kB of stack memory */
}
