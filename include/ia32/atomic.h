/*
  include/ia32/atomic.h
  Copyright (c) 2005 Luke McCarthy

  Atomic memory access.
*/

#ifndef IA32_ATOMIC_H
#define IA32_ATOMIC_H

static inline void atomic_inc(uint32 *var)
{
	__asm__ __volatile__ ("lock incl %0" : "=m"(*var));
}

static inline void atomic_dec(uint32 *var)
{
	__asm__ __volatile__ ("lock decl %0" : "=m"(*var));
}

static inline void atomic_add(uint32 *var, uint32 by)
{
	__asm__ __volatile__ ("lock addl %1, %0" : "=m"(*var) : "r"(by));
}

/* battleships beware! */
#define atomic_sub(var, by) atomic_add((var), -(by))

#endif /* IA32_ATOMIC_H */
