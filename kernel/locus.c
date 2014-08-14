/*
  kernel/locus.c
  Copyright (c) 2005 Luke McCarthy
*/

#include <locus.h>
#include <types.h>
#include <pools.h>
#include <pagealloc.h>
#include <lib/assert.h>
#include <lib/ptrcast.h>

uint locus_count = 1;
struct locus *locus_current = &idle_locus;

struct locus *locus_put(struct locus *l)
{
	ASSERT(l);
	l->next = locus_current->next;
	l->next->prev = l;
	l->prev = locus_current;
	l->prev->next = l;
	locus_count++;
	return l;
}

void locus_stop(struct locus *l)
{
	ASSERT(l);
	l->status = LOCUS_EXITED;
}

static void destroy(struct locus *l)
{
	l->prev->next = l->next;
	l->next->prev = l->prev;
	if (l->ksp)
		kpage_free(l->ksp);
	free(l);
	locus_count--;
}

struct locus *locus_next()
{
	uint i;
	struct locus *next, *l = locus_current->next;

	for (i = 0; i < locus_count; i++, l = next) {
		next = l->next;
		switch (l->status) {
			case LOCUS_READY:
				return l;
			case LOCUS_EXITED:
				destroy(l);
				break;
			default:
				break;
		}
	}
	return &idle_locus;
}

struct locus *locus_run(struct locus *l)
{
	ASSERT(l);
	l->status = LOCUS_READY;
	return l;
}
