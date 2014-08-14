/*
  include/pagealloc.h
  Copyright (c) 2005 Luke McCarthy
*/

#ifndef PAGEALLOC_H
#define PAGEALLOC_H

#include <types.h>

extern addr_t page_alloc();
extern void page_free(addr_t p);
extern uint nr_pages_free();

extern void *kpage_alloc();
extern void kpage_free(void *ptr);
extern void *kmap(addr_t p);
extern addr_t kunmap(void *v);
extern addr_t ktophys(void *v);

extern void page_fill(void *dst, uint ptn);
extern void page_copy(void *dst, void *src);

#endif /* PAGEALLOC_H */
