/*
  include/memory.h
  Copyright (c) 2005 Luke McCarthy

  Low-level memory information.
*/

#ifndef MEMORY_H
#define MEMORY_H

#include ARCH(memory.h)

/*
  NOTE: Only applies to static kernel addresses
*/
#define TOPHYS(addr) PTR(UVAL(addr) - KERN_DISP)
#define TOVIRT(addr) PTR(UVAL(addr) + KERN_DISP)

/*
  Make these private to page allocator?
*/
extern char
	_img   [], _eimg   [],
	_rodata[], _erodata[],
	_text  [], _etext  [],
	_data  [], _edata  [],
	_bss   [], _ebss   [],
	_stack [], _estack [];

#endif /* MEMORY_H */
