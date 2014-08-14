/*
  include/ia32/debug.h
  Copyright (c) 2005 Luke McCarthy

  Debug print helpers.
*/

#ifndef IA32_DEBUG_H
#define IA32_DEBUG_H

#include <types.h>
#include <ia32/interrupt.h>

extern void print_selector(uint16 sel);
extern void print_pushad(struct pushad *regs);
extern void print_iretd(struct iretd *iregs);
extern void print_trap(struct trap *frame);

#endif /* IA32_DEBUG_H */
