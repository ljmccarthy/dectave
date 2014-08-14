/*
  include/pools.h
  Copyright (c) 2005 Luke McCarthy

  Kernel structure memory pools.
*/

#ifndef POOLS_H
#define POOLS_H

#include <reca.h>

extern struct reca locus_pool, process_pool;

#define alloc_locus() ((struct locus *) reca_alloc(&locus_pool))
#define alloc_process() ((struct process *) reca_alloc(&process_pool))
#define free(p) reca_free(p)

#endif /* POOLS_H */
