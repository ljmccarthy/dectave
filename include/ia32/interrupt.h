/*
  include/ia32/interrupt.h
  Copyright (c) 2005 Luke McCarthy

  Interrupt handling.
*/

#ifndef IA32_INTERRUPT_H
#define IA32_INTERRUPT_H

#include <macros.h>
#include <types.h>

#define NUM_TRAPS 20
#define NUM_IRQS  16
#define SYSCALL   48
#define NUM_INTRS 49

struct pushad {
	uint32 edi, esi, ebp, esp, ebx, edx, ecx, eax;
};

struct iretd {
	uint32 eip, cs, eflags, esp, ss;
};

struct iretd_kern {
	uint32 eip, cs, eflags;
};

struct trap {
//	uint32 ds;
	struct pushad regs;
	uint32 num, errcode;
	struct iretd iregs;
};

struct trap_kern {
//	uint32 ds;
	struct pushad regs;
	uint32 num, errcode;
	struct iretd_kern iregs;
};

struct syscall {
//	uint32 ds;
	struct pushad regs;
	struct iretd iregs;
};

static inline void cli()  /* disable interrupts */
{
	__asm__ ("cli");
}

static inline void sti()  /* enable interrupts */
{
	__asm__ ("sti");
}

extern uint32 lock_level;

#define lock() DO      \
	cli();         \
	lock_level++;  \
END

#define unlock() DO           \
	lock_level--;         \
	if (lock_level == 0)  \
		sti();        \
END

#endif /* IA32_INTERRUPT_H */
