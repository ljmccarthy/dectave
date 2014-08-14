/*
  kernel/pools.c
  Copyright (c) 2005 Luke McCarthy

  Kernel structure memory pools.
*/

#include <reca.h>
#include <locus.h>
#include <process.h>

struct reca locus_pool, process_pool;

void init_pools()
{
	reca_init(&locus_pool, sizeof(struct locus), "locus");
	reca_init(&process_pool, sizeof(struct process), "process");
}
