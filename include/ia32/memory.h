/*
  include/ia32/memory.h
  Copyright (c) 2005 Luke McCarthy

  Low-level memory information and operations.
*/

#ifndef IA32_MEMORY_H
#define IA32_MEMORY_H

#include <types.h>
#include <lib/ptrcast.h>

#define KERN_PHYS 0x00100000  /* first 1MB */
#define KERN_VIRT 0xFF100000  /* top 4MB */
#define KERN_DISP (KERN_VIRT-KERN_PHYS)

#endif /* IA32_MEMORY_H */
