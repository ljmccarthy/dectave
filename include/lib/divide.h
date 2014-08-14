/*
  include/lib/divide.h
  Copyright (c) 2005 Luke McCarthy
*/

#ifndef DIVIDE_H
#define DIVIDE_H

#include <types.h>
#include <macros.h>

PURE static inline intptr idivup(intptr x, intptr y)
{
	return x / y + (x % y != 0);
}

PURE static inline uintptr udivup(uintptr x, uintptr y)
{
	return x / y + (x % y != 0);
}

#endif /* DIVIDE_H */
