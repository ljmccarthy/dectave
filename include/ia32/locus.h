/*
  include/ia32/locus.h
  Copyright (c) 2005 Luke McCarthy

  Locus definitions for ia32.
*/

#ifndef IA32_LOCUS_H
#define IA32_LOCUS_H

#include <types.h>
#include <ia32/interrupt.h>

/*
  The size and shape of this structure
  is assumed in arch/ia32/start.asm
*/
struct thread_context {
	struct pushad regs;
	struct iretd iregs;
	uint32 cr3;
};

#define is_user_locus(l) ((l)->ctx.iregs.cs & 3)
#define is_kern_locus(l) (!is_user_locus(l))

#endif /* IA32_LOCUS_H */
