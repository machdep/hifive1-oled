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
		PROVIDE(sfont = .);
		INCLUDE "FONT_PATH";
	} > flash

	/* Ensure _smem is associated with the next section */
	. = .;
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
}
