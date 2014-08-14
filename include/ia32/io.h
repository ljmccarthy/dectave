/*
  include/ia32/io.h
  Copyright (c) 2005 Luke McCarthy

  Port I/O.
*/

#ifndef IA32_IO_H
#define IA32_IO_H

#include <types.h>

static inline void out8(uint16 port, uint8 value)
{
	__asm__ __volatile__ ("out %b0, %w1" : : "a"(value), "Nd"(port));
}

static inline uint8 in8(uint16 port)
{
	register uint8 value;
	__asm__ __volatile__ ("in %w1, %b0" : "=a"(value) : "Nd"(port));
	return value;
}

static inline void out32(uint16 port, uint32 value)
{
	__asm__ __volatile__ ("out %0, %w1" : : "a"(value), "Nd"(port));
}

static inline uint32 in32(uint16 port)
{
	register uint32 value;
	__asm__ __volatile__ ("in %w1, %0" : "=a"(value) : "Nd"(port));
	return value;
}

#endif /* IA32_IO_H */
