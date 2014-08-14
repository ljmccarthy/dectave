/*
  include/ia32/eflags.h
  Copyright (c) 2005 Luke McCarthy

  EFLAGS regiser.
*/

#ifndef IA32_EFLAGS_H
#define IA32_EFLAGS_H

#include <macros.h>

#define EFL_NULL 0x2  /* bit 1 must be set */

#define EFL_CF   BIT(0)   /* carry */
#define EFL_PF   BIT(2)
#define EFL_AF   BIT(4)
#define EFL_ZF   BIT(6)   /* zero */
#define EFL_SF   BIT(7)   /* sign */
#define EFL_TF   BIT(8)   /* trap flag */
#define EFL_IF   BIT(9)   /* interrupt enable */
#define EFL_DF   BIT(10)  /* direction */
#define EFL_OF   BIT(11)  /* overflow */
#define EFL_NT   BIT(14)  /* nested task */
#define EFL_RF   BIT(16)  /* resume flag */
#define EFL_VM   BIT(17)  /* virtual 8086 mode */
#define EFL_AC   BIT(18)  /* alignment check */
#define ELF_VIF  BIT(19)  /* virtual interrupt flag */
#define EFL_VIP  BIT(20)  /* virtual interrupt pending */
#define EFL_ID   BIT(21)  /* identification flag (CPUID) */

#define EFL_IOPL(n) ((UVAL(n)&3)<<12)  /* I/O privilege level */

#endif /* IA32_EFLAGS_H */
