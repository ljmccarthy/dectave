/*
  arch/ia32/process.c
  Copyright (c) 2005 Luke McCarthy
*/

#include <process.h>
#include <pools.h>
#include <paging.h>
#include <pagealloc.h>
#include <memory.h>
#include <macros.h>
#include <halt.h>
#include <console.h>
#include <lib/assert.h>
#include <lib/divide.h>

void ps_select(struct process *ps)
{
	uint32 dir = ktophys(ps->dir);
	if (cr3_get() != dir)
		cr3_set(dir);
}

static void map_kernel(drtab_t dir)
{
	uint32 pgt = UVAL(TOPHYS(kern_pgtab)) | PG_PRESENT | PG_WRITABLE;

	dir[0x3FC] = pgt;
	dir[0x3FD] = pgt + 0x1000;
	dir[0x3FE] = pgt + 0x2000;
	dir[0x3FF] = pgt + 0x3000;
}

struct process *ps_create()
{
	struct process *ps;

	ps = alloc_process();
	if (!ps)
		return NULL;

	ps->dir = kpage_alloc();
	if (!ps->dir)
		goto err;

	page_fill(ps->dir, 0);
	map_kernel(ps->dir);
	return ps;

err:
	free(ps);
	return NULL;
}

static void pgt_commit(drtab_t dir, uint ti, uint start, uint end)
{
	uint32 *pgt;
	uint32 attr = PG_PRESENT | PG_WRITABLE | PG_USERMODE;

#if 0
	print("pgt_commit(dir="); print_ptr(dir);
	print(", ti="); print_hex(ti);
	print(", start="); print_hex(start);
	print(", end="); print_hex(end);
	print(")\n");
#endif

	if (dir[ti])
		pgt = kmap(dir[ti] & PAGE_MASK);
	else {
		pgt = kpage_alloc();
		page_fill(pgt, 0);
		dir[ti] = ktophys(pgt) | attr;
	}

	ASSERT(pgt);

	for (uint i = start; i < end; i++) {
		pgt[i] = page_alloc() | attr;
		ASSERT(pgt[i] & PAGE_MASK);
	}
	kunmap(pgt);
}

static void assert_range(addr_t vaddr, size_t size)
{
	ASSERT(!(vaddr % PAGE_SIZE));
	ASSERT(!(size % PAGE_SIZE));
	ASSERT(vaddr != 0);
	ASSERT(vaddr + size < UVAL(TOVIRT(0)));
}

int ps_commit(struct process *ps, addr_t vaddr, size_t size)
{
	uint off, end;

#if 0
	print("ps_commit(ps="); print_ptr(ps);
	print(", vaddr="); print_ptr(vaddr);
	print(", size="); print_hex(size);
	print(")\n");
#endif

	assert_range(vaddr, size);

	off = vaddr / 0x1000;
	end = (vaddr + size) / 0x1000;

	if (off / 0x400 == end / 0x400)
		pgt_commit(ps->dir, off / 0x400, off % 0x400, end % 0x400);
	else {
		if (off % 0x400)
			pgt_commit(ps->dir, off / 0x400, off % 0x400, 0x400);

		if (end % 0x400)
			pgt_commit(ps->dir, end / 0x400, 0, end % 0x400);

		for (uint i = udivup(off, 0x400); i < end / 0x400; i++)
			pgt_commit(ps->dir, i, 0, 0x400);
	}
	return 1;
}
