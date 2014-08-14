/*
  include/paging.h
  Copyright (c) 2005 Luke McCarthy

  Page table and directory flags.
*/

#ifndef PAGING_H
#define PAGING_H

#include <macros.h>
#include ARCH(paging.h)

#define PAGE_SIZE (1 << PAGE_SHIFT)
#define PAGE_MASK (~(PAGE_SIZE-1))
#define PAGE_ALIGN(x) ((UVAL(x) + PAGE_SIZE-1) & PAGE_MASK)

#endif /* PAGING_H */
