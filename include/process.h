/*
  include/process.h
  Copyright (c) 2005 Luke McCarthy
*/

#ifndef PROCESS_H
#define PROCESS_H

#include <paging.h>
#include <types.h>

struct process {
	drtab_t dir;  /* virtual kernel address */
};

extern struct process *ps_create();
extern int ps_commit(struct process *ps, addr_t vaddr, size_t size);
extern void ps_select(struct process *ps);

#endif /* PROCESS_H */
