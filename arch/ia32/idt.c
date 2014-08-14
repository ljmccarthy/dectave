/*
  arch/ia32/idt.c
  Copyright (c) 2005 Luke McCarthy

  Interrupt descriptor table initialisation (called in start.asm).
*/

#include <macros.h>
#include <types.h>
#include <console.h>
#include <ia32/idt.h>
#include <ia32/interrupt.h>
#include <ia32/segment.h>

extern uint32 trap_entry_tab[NUM_TRAPS], irq_entry_tab[NUM_IRQS];
extern void syscall_entry_point();

#define IDT_SET(_vec, _handler, _sel, _type) do {  \
	idt[_vec].offset_lo = UVAL(_handler);      \
	idt[_vec].offset_hi = UVAL(_handler)>>16;  \
	idt[_vec].unused = 0;                      \
	idt[_vec].type = (_type);                  \
	idt[_vec].selector = (_sel);               \
} while (0)

#define IDT_NULL(_vec) do { *(uint64*)&idt[_vec] = 0; } while (0)

void idt_init()
{
	int i;

	for (i = 0; i < 20; i++)
		IDT_SET(i, &trap_entry_tab[i], KERN_CS,
			IDT_TRAPGATE | IDT_PRESENT | IDT_32BIT | IDT_DPLKERN);

	for (i = 20; i < 32; i++)
		IDT_NULL(i);

	for (i = 32; i < 48; i++)
		IDT_SET(i, &irq_entry_tab[i-32], KERN_CS,
			IDT_INTRGATE | IDT_PRESENT | IDT_32BIT | IDT_DPLKERN);

	IDT_SET(48, syscall_entry_point, KERN_CS,
		IDT_INTRGATE | IDT_PRESENT | IDT_32BIT | IDT_DPLUSER);
}
