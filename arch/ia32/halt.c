/*
  arch/ia32/halt.c
  Copyright (c) 2005 Luke McCarthy

  System-wide halt and reboot.
*/

#include <halt.h>
#include <console.h>
#include <ia32/interrupt.h>
#include <ia32/io.h>

void halt()
{
	cli();
	while (1)
		;
}

void reboot()
{
	cli();
	print("\nRebooting...");
	out8(0x64, 0xfe);
	while (1)
		;
}
