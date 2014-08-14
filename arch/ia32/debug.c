/*
  arch/ia32/debug.c
  Copyright (c) 2005 Luke McCarthy

  Debug print helpers.
*/

#include <console.h>
#include <ia32/debug.h>
#include <ia32/interrupt.h>
#include <ia32/paging.h>

void print_selector(uint16 sel)
{
	if (sel & ~7) {
		print_hex(sel>>3);
		putchar(':');
		print_hex(sel & 3);
	}
	else
		print("nil");
}

void print_pushad(struct pushad *regs)
{
	print("  ");
	print("eax="); print_ptr(regs->eax); sp();
	print("ecx="); print_ptr(regs->ecx); sp();
	print("edx="); print_ptr(regs->edx); sp();
	print("ebx="); print_ptr(regs->ebx); nl(); print("  ");
	print("esp="); print_ptr(regs->esp); sp();
	print("ebp="); print_ptr(regs->ebp); sp();
	print("esi="); print_ptr(regs->esi); sp();
	print("edi="); print_ptr(regs->edi);
}

void print_iretd(struct iretd *iregs)
{
	print("  ");
	print("efl="); print_ptr(iregs->eflags); sp();
	print("cs="); print_selector(iregs->cs); sp();
	print("eip="); print_ptr(iregs->eip); sp();
	if (iregs->cs & 3) {
		print("ss="); print_selector(iregs->ss); sp();
		print("esp="); print_ptr(iregs->esp); sp();
	}
}

void print_trap(struct trap *frame)
{
	print("\nTrap "); print_deci(frame->num);
	putchar(':'); print_uint(frame->errcode, 2); nl();
	print_pushad(&frame->regs); nl();
	print_iretd(&frame->iregs); nl();
	print("  cr2="); print_ptr(cr2_get());
	print(" cr3="); print_ptr(cr3_get());
}
