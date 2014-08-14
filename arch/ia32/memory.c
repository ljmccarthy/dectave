/*
  arch/ia32/memory.c
  Copyright (c) 2005 Luke McCarthy

  Memory initialisation.
*/

#include <memory.h>
#include <types.h>
#include <console.h>
#include <pagealloc.h>
#include <halt.h>
#include <paging.h>
#include <bittree.h>
#include <lib/string.h>
#include <lib/assert.h>
#include <lib/round.h>
#include <lib/divide.h>
#include <ia32/bitmap.h>
#include <ia32/multiboot.h>
#include <ia32/memory.h>

extern char _lowfreepre[], _elowfree[];

/*
  New page allocator

  * Uses bitmaps
  * No longer requires direct access to physical memory
  * Supports 4 MB pages
  * Kernel pages mapped dynamically into kernel address space
  * Possible to extend to support PAE > 4 GB

  The bitmaps are statically allocated in low memory (see arch/ia32/kernel.ld).
  The pointer _lowfree is modified so that bitmap_4k4 does not waste memory
  for machines with less than 4 GB of physical memory.
*/

struct boot_info *mbinf;  /* initialised by boot code */
static char *_lowfree = TOVIRT(_lowfreepre);
static size_t memsize;
static struct bittree memory, mem_4m, kspace;

/*
  Boot-time allocation for bitmaps in low memory.
*/

static void *boot_alloc(size_t size)
{
	void *p = _lowfree;
	_lowfree += size;
	return p;
}

static void boot_alloc_fini()
{
	_lowfree = TOPHYS(PAGE_ALIGN(_lowfree));
	ASSERT(_lowfree < _elowfree);
}

#define ceil4(x) (((x) + 3) & ~3)

static void boot_alloc_bittree(struct bittree *btr, uint32 num)
{
	btr->free = 0;
	btr->b1 = 0;

	num = udivup(num, 8);
	btr->b4 = boot_alloc(ceil4(num));
	memset(btr->b4, 0, ceil4(num));

	num = udivup(num, 32);
	btr->b3 = boot_alloc(ceil4(num));
	memset(btr->b3, 0, ceil4(num));

	num = udivup(num, 32);
	btr->b2 = boot_alloc(ceil4(num));
	memset(btr->b2, 0, ceil4(num));
}

void setup_bitmaps()
{
	boot_alloc_bittree(&memory, memsize / PAGE_SIZE);
	boot_alloc_bittree(&mem_4m, memsize / 0x400000);
	boot_alloc_bittree(&kspace, 0x1000000 / PAGE_SIZE);
	boot_alloc_fini();
}

uint nr_pages_free()
{
	return mem_4m.free * 0x400 + memory.free;
}

addr_t page_alloc()
{
	if (memory.free == 0) {
		if (mem_4m.free == 0)
			return 0;
		uint32 i = btr_alloc(&mem_4m) * 0x400;
		btr_markfree(&memory, i, i + 0x400);
	}
#if 1
	return btr_alloc(&memory) * PAGE_SIZE;
#else
	addr_t p = btr_alloc(&memory) * PAGE_SIZE;
	print("page_alloc: "); print_ptr(p); nl();
	return p;
#endif
}

/*
  TODO: Free 4 MB region when possible.
*/
void page_free(addr_t p)
{
	btr_free(&memory, p / PAGE_SIZE);
}

void *kmap(addr_t p)
{
	uint32 i;
	void *v;

	if (kspace.free == 0)
		return NULL;

	i = btr_alloc(&kspace);
	v = TOVIRT(i * PAGE_SIZE);

	ASSERT(v >= TOVIRT(0));
	ASSERT(CPTR(v) >= _eimg || CPTR(v) < _img);
	ASSERT(!(UVAL(p) & ~PAGE_MASK));
	ASSERT(!(UVAL(v) & ~PAGE_MASK));

	kern_pgtab[i] = p | PG_PRESENT | PG_WRITABLE;
	invlpg(v);
#if 0
	print("kmap: "); print_ptr(p); print(" -> ");
	print_ptr(v); print(" ("); print_hex(i); print(")\n");
#endif
	return v;
}

addr_t kunmap(void *v)
{
	uint32 i = UVAL(TOPHYS(v)) / PAGE_SIZE;
	addr_t p = kern_pgtab[i] & PAGE_MASK;
	btr_free(&kspace, i);
	kern_pgtab[i] = 0;
//	invlpg(v);
#if 0
	print("kunmap: "); print_ptr(p); print(" -> ");
	print_ptr(v); print(" ("); print_hex(i); print(")\n");
#endif
	return p;
}

addr_t ktophys(void *v)
{
	return kern_pgtab[UVAL(TOPHYS(v)) / PAGE_SIZE] & PAGE_MASK;
}

void *kpage_alloc()
{
	if (nr_pages_free() == 0 || kspace.free == 0)
		return NULL;
	else
		return kmap(page_alloc());
}

void kpage_free(void *v)
{
	page_free(kunmap(v));
}

static void setup_kspace()
{
	/* 16 MB kernel address space */
	btr_markfree(&kspace,
	             UVAL(TOPHYS(_eimg)) / PAGE_SIZE, 0x1000000 / PAGE_SIZE);
}

static void print_claim(uint32 off, uint32 end)
{
	print("* Claiming physical memory at ");
	print_ptr(off); putchar('-'); print_ptr(end - 1);
	print(" ("); print_size(end - off); print(")\n");
}

static void markclaim(uint32 off, uint32 end)
{
	print_claim(off * PAGE_SIZE, end * PAGE_SIZE);
	btr_markfree(&memory, off, end);
}

static void setup_memory()
{
	print("* Detected ");
	print_size(memsize);
	print(" of volatile memory\n");

	/*
	  To simplify calculations we assume, or course, that
	  the kernel image does not exceed the first 4 MB.
	*/
	ASSERT(UVAL(TOPHYS(_eimg)) < 0x400000);

	/* 4 KB pages in low memory */
	markclaim(UVAL(_lowfree) / PAGE_SIZE, UVAL(_elowfree) / PAGE_SIZE);

	/* 4 KB pages after kernel in first 4 MB */
	markclaim(UVAL(TOPHYS(_eimg)) / PAGE_SIZE, 0x400);

	/* 4 MB pages starting from second 4 MB */
	uint32 top4m = 1;
	uint32 end4m = memsize / 0x400000;

	print_claim(top4m * 0x400000, end4m * 0x400000);
	btr_markfree(&mem_4m, top4m, end4m);

	/* Mark after last 4 MB for 4 KB pages (if any) */
	if (memsize % 0x400000)
		markclaim(end4m * 0x400, memsize / PAGE_SIZE);
}

static void delete_temp_maps()
{
	kern_drtab[0x000] = 0;
	kern_drtab[0x001] = 0;
	kern_drtab[0x002] = 0;
	kern_drtab[0x003] = 0;
//	cr3_set(cr3_get());
}

/*
  Must be called by main() once and only once.
  Uses multiboot information to set up the page allocator.
*/
void init_memory()
{
	ASSERT(mbinf);

	foreach_memmap (m) if (
	   m->base_addr_lo == 0x100000
	&& m->type == 1
	&& m->base_addr_hi == 0
	&& m->length_hi == 0) {  /* memories > 4 GB not yet supported */
		memsize = (m->length_lo & PAGE_MASK) + 0x100000;
		setup_bitmaps();
		setup_memory();
		setup_kspace();
		delete_temp_maps();
#if 0
		void *p;
		do {
			p = kpage_alloc();
			print_ptr(p); sp();
			print_deci(kspace.free); nl();
			waitkey();
		} while (p);
#endif
		return;
	}
	die("Can't find main free memory region");
}
