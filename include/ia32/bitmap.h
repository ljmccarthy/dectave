/*
  include/ia32/bitmap.h
  Copyright (c) 2005 Luke McCarthy

  Bitmap operations.
*/

#ifndef IA32_BITMAP_H
#define IA32_BITMAP_H

#include <types.h>

static inline uint32 ffs(uint32 bits)
{
	register uint32 ret;
	__asm__ ("bsf %1, %0" : "=r"(ret) : "r"(bits));
	return ret;
}

static inline void set(uint32 *p, uint32 i)
{
	__asm__ ("bts %1, %0" : "=m"(p[i/32]) : "r"(i%32) : "memory");
}

static inline void clr(uint32 *p, uint32 i)
{
	__asm__ ("btr %1, %0" : "=m"(p[i/32]) : "r"(i%32) : "memory");
}

#endif /* IA32_BITMAP_H */
