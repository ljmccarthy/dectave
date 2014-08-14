/*
  include/halt.h
  Copyright (c) 2005 Luke McCarthy

  System-wide halt and reboot.
*/

#ifndef HALT_H
#define HALT_H

#include <macros.h>

extern NORETURN void halt();
extern NORETURN void reboot();
extern NORETURN void die(char *msg);

#endif /* HALT_H */
