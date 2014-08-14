/*
  kernel/reca.c
  Copyright (c) 2005 Luke McCarthy

  Record allocator from libspa.
*/

#include <reca.h>
#include <paging.h>
#include <pagealloc.h>
#include <macros.h>
#include <types.h>
#include <lib/string.h>
#include <lib/round.h>
#include <lib/clip.h>
#include <lib/ptrcast.h>

#define region __reca_region

#define ALIGNMENT sizeof(void *)
#define REGION_SIZE PAGE_SIZE
#define REGION_AVAIL (REGION_SIZE - sizeof(struct region))

#define STAT_ALLOC_NEW(rp) DO (rp)->stats.allocs_new++ END
#define STAT_ALLOC_FL(rp)  DO (rp)->stats.allocs_fl++  END
#define STAT_FREE(rp)      DO (rp)->stats.frees++      END
#define STAT_VM_ALLOC(rp)  DO (rp)->stats.vm_allocs++  END
#define STAT_VM_FREE(rp)   DO (rp)->stats.vm_frees++   END

struct freelist {         /* list of free records, one per region */
	struct freelist *next;
};

struct region {           /* allocated at the end of each region */
	struct reca *reca;
	struct region *prev;
	struct region *next;
	struct freelist *flp;
	uint ctfree;
};

/* The base address of the region from which p was allocated. */
PURE static inline char *region_base_addr(void *p)
{
	return CPTR(UVAL(p) & ~(REGION_SIZE-1));
}

/* The region header for the region from which p was allocated. */
PURE static inline struct region *region_of(void *p)
{
	return PTR(region_base_addr(p) + REGION_AVAIL);
}

int reca_init(struct reca *r, size_t recsize, const char *name)
/*
  Initialises a record pool called name for records of size recsize.
  It is assumed that the the record pool is not already in use.
*/
{
	if (recsize > REGION_AVAIL)
		return -1;
	memset(r, 0, sizeof(struct reca));
	r->recsize = uceil(umax(recsize, sizeof(struct freelist)), ALIGNMENT);
	r->recper = REGION_AVAIL / r->recsize;
	r->stats.name = name ? name : "";
	return 0;
}

/* Unlinks a region from the list of regions with freed records. */
static void unlink_region_free(struct reca *r, struct region *rg)
{
	if (rg->prev)
		rg->prev->next = rg->next;
	else
		r->rgflp = rg->next;
	if (rg->next)
		rg->next->prev = rg->prev;
}

/* Links a region in to the list of regions with freed records. */
static inline void relink_region_free(struct reca *r, struct region *rg)
{
	if (!r->rgflp) {
		/* Empty list, push on to top. */
		rg->next = NULL;
		rg->prev = NULL;
		r->rgflp = rg;
	}
	else {
	/*
	  Non-empty, tuck under head of list.
	  This makes sure allocation stays to the
	  same region until its free list is
	  empied. This increases the likelyhood
	  that allocations aren't scattered sparsely
	  across regions which cannot be reclaimed.
	*/
		rg->next = r->rgflp->next;
		rg->prev = r->rgflp;
		if (r->rgflp->next)
			r->rgflp->next->prev = rg;
		r->rgflp->next = rg;
	}
}

/* Unlinks a region from the list of saturated regions. */
static inline void unlink_region_satu(struct reca *r, struct region *rg)
{
	if (rg->prev)
		rg->prev->next = rg->next;
	else
		r->rgslp = rg->next;
	if (rg->next)
		rg->next->prev = rg->prev;
}

/* Links a region from the list of saturated regions. */
static void relink_region_satu(struct reca *r, struct region *rg)
{
	rg->next = r->rgslp;
	rg->prev = NULL;
	if (r->rgslp)
		r->rgslp->prev = rg;
	r->rgslp = rg;
}

/*
  vm_allocs a new region for reca_alloc and links it to
  the saturated list.
*/
static inline void alloc_region(struct reca *r)
{
	r->rgptr = kpage_alloc();
	if (!r->rgptr)
		r->rgend = NULL;
	else {
		r->rgend = r->rgptr + REGION_AVAIL + 1;
		memset(region_of(r->rgptr), 0, sizeof(struct region));
		region_of(r->rgptr)->reca = r;
		relink_region_satu(r, region_of(r->rgptr));
		STAT_VM_ALLOC(r);
	}
}

static inline void *alloc_fl(struct reca *r)
{
	struct region *rg = r->rgflp;
	void *p = rg->flp;

	rg->flp = rg->flp->next;
	if (--rg->ctfree == 0) {  /* No more free records on this region? */
		unlink_region_free(r, rg);
		relink_region_satu(r, rg);
	}
	STAT_ALLOC_FL(r);
	return p;
}

static inline void *alloc_new(struct reca *r)
{
	/*
	  Allocate a new region if necessary.
	  Clever trick: true also if rgptr and rgend are NULL.
	  Check for overflow!
	*/
	if (r->rgptr + r->recsize > r->rgend
	 || r->rgptr + r->recsize < r->rgptr) {
		alloc_region(r);
		if (!r->rgptr)
			return NULL;
	}
	/* Allocate linearly. */
	void *p = r->rgptr;
	r->rgptr += r->recsize;
	STAT_ALLOC_NEW(r);
	return p;
}

/* Allocate a record from the pool. Memory is uninitialised. */
void *reca_alloc(struct reca *r)
{
	return r->rgflp ? alloc_fl(r) : alloc_new(r);
}

/* Free record p's memory. */
void reca_free(void *p)
{
	struct region *rg = region_of(p);
	struct reca *r = rg->reca;

	if (++rg->ctfree == r->recper) {  /* All records are freed. */
		unlink_region_free(r, rg);
		kpage_free(region_base_addr(p));
		STAT_VM_FREE(r);
	}
	else {
		/* Insert into freelist. */
		((struct freelist*)p)->next = rg->flp;
		rg->flp = p;
		if (rg->ctfree == 1) {  /* Not already in region freelist? */
			unlink_region_satu(r, rg);
			relink_region_free(r, rg);
		}
	}
	STAT_FREE(r);
}

static inline void free_region_list(struct reca *r, struct region *rg)
{
	while (rg) {
		struct region *rgnext = rg->next;
		kpage_free(region_base_addr(rg));
		STAT_VM_FREE(r);
		rg = rgnext;
	}
}

/*
  Frees all allocated records from a pool.
  The pool can be used again without re-initialising.
  However, it may be re-initialised with reca_init
  if a different record size is needed, or to reset
  the statistics.
*/
void reca_fini(struct reca *r)
{
	free_region_list(r, r->rgflp);
	free_region_list(r, r->rgslp);
	r->stats.frees = r->stats.allocs_new + r->stats.allocs_fl;
	r->rgflp = NULL;
	r->rgslp = NULL;
	r->rgptr = NULL;
	r->rgend = NULL;
}

/* The returned struct exists as long as it's reca exists. */
struct reca_stats *reca_stats(struct reca *r)
{
	return &r->stats;
}
