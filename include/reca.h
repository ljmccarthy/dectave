/*
  include/reca.h
  Copyright (c) 2005 Luke McCarthy

  Record allocator from libspa.
*/

#ifndef RECA_H
#define RECA_H

#include <types.h>

struct reca_stats {
	const char *name;         /* optional user-assigned name */
	unsigned int allocs_new;  /* allocated new */
	unsigned int allocs_fl;   /* allocated from free list */
	unsigned int frees;       /* freed records */
	unsigned int vm_allocs;   /* vm alloc requests */
	unsigned int vm_frees;    /* vm frees */
};

/* PRIVATE */
struct reca {
	size_t recsize, recper;
	char *rgptr, *rgend;
	struct __reca_region *rgflp, *rgslp;
	struct reca_stats stats;
};

extern int reca_init(struct reca *rp, size_t size, const char *name);
extern void reca_fini(struct reca *rp);
extern void *reca_alloc(struct reca *rp);
extern void reca_free(void *p);
extern struct reca_stats *reca_stats(struct reca *rp);

#endif /* RECA_H */
