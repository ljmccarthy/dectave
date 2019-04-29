/*
  include/ia32/idt.h
  Copyright (c) 2005 Luke McCarthy <luke@iogopro.co.uk>

  Interrupt descriptor table.
*/

#ifndef IA32_IDT_H
#define IA32_IDT_H

#include <macros.h>
#include <types.h>
#include <ia32/interrupt.h>

#define IDT_TASKGATE 0x5
#define IDT_INTRGATE 0x6
#define IDT_TRAPGATE 0x7
#define IDT_32BIT   (1<<3)
#define IDT_PRESENT (1<<7)
#define IDT_DPL(n)  ((n)<<5)
#define IDT_DPLUSER IDT_DPL(3)
#define IDT_DPLKERN IDT_DPL(0)

/*
  task/interrupt/trap gate descriptor
*/
struct PACKED idt_gate {
	uint16 offset_lo;
	uint16 selector;
	uint8 unused;
	uint8 type;
	uint16 offset_hi;
};

extern struct idt_gate idt[NUM_INTRS];

#endif /* IA32_IDT_H */
