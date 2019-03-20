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
		PROVIDE(_sfont = .);
		INCLUDE "FONT_PATH";
		PROVIDE(_efont = .);
	} > flash

	PROVIDE(_smem = .);
	.sdata : {
		PROVIDE(_sdata = .);
		*(.sdata)
		PROVIDE(_edata = .);
	} > sram AT > flash

	.bss : {
		PROVIDE(_sbss = .);
		*(.bss COMMON)
		*(.sbss)
		PROVIDE(_ebss = .);
	} > sram

	. = ALIGN(4);

	stack_top = . + 0x1000; /* 4kB of stack memory */
}
