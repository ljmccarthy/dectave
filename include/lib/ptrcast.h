/*
  lib/ptrcast.h
  Copyright (c) 2005 Luke McCarthy
*/

#ifndef UTIL_PTRCAST_H
#define UTIL_PTRCAST_H

#include <types.h>

#define PTR(x) ((void *)(x))
#define CPTR(x) ((char *)(x))
#define SVAL(x) ((intptr)(x))
#define UVAL(x) ((uintptr)(x))
#define ADDR(x,y) PTR(CPTR(x)+UVAL(y))

#endif /* UTIL_PTRCAST_H */
