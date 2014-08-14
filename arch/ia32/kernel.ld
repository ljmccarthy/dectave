/*
  arch/ia32/kernel.ld
  Copyright (c) 2005 Luke McCarthy

  Kernel image linker script.
*/

OUTPUT_FORMAT(elf32-i386)
ENTRY(_start)

_phys = 0x00100000;
_virt = 0xFF100000;
_disp = _virt - _phys;

kern_drtab  = 0x0000 + 0xFF000000;
kern_pgtab  = 0x1000 + 0xFF000000;
_lowfreepre = 0x5000;  /* _lowfree is somewhere past here */
_elowfree   = 0xa0000;

SECTIONS {
	. = _virt;
	_img = .;
	.rodata : AT(_phys) {
		_rodata = .;
		KEEP(*(.rodata))
		_erodata = .;
	}
	.text ALIGN(32) : AT(ADDR(.text) - _disp) {
		_text = .;
		KEEP(*(.text))
		_etext = .;
	}
	.data ALIGN(32) : AT(ADDR(.data) - _disp) {
		_data = .;
		KEEP(*(.data))
		_edata = .;
	}
	.bss ALIGN(32) : AT(ADDR(.bss) - _disp) {
		_bss = .;
		KEEP(*(COMMON))
		KEEP(*(.bss))
		_ebss = .;
	}

	. = ALIGN(8);
	idt = .;
	. = . + (8 * 49);

	tss = .;
	. = . + 0x68;

	. = ALIGN(0x800);
	_stack = .;
	. = . + 0x800;
	_estack = .;

	_intstack = .;
	. = . + 0x800;
	_eintstack = .;

	. = ALIGN(0x1000);  /* _eimg MUST be page aligned! */
	_eimg = .;
}
