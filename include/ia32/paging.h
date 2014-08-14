/*
  include/ia32/paging.h
  Copyright (c) 2005 Luke McCarthy

  Low-level page table and directory flags.
*/

#ifndef IA32_PAGING_H
#define IA32_PAGING_H

#include <macros.h>
#include <types.h>

typedef struct { uint32 pde; } pgd_t;
typedef struct { uint32 pte; } pgt_t;

#define PGD(x) ((pgd_t) { (x) } )
#define PGT(x) ((pgt_t) { (x) } )

/*
  Page table/directory entry bits

  PG_ for tables and directories
  PT_ for tables only
  PD_ for directories only
*/
#define PG_PRESENT   BIT(0)   /* clear for non-present */
#define PG_WRITABLE  BIT(1)   /* clear for read-only */
#define PG_USERMODE  BIT(2)   /* clear for system pages */
#define PG_WRITETHRU BIT(3)   /* clear for write-back */
#define PG_NOCACHE   BIT(4)   /* clear for cache enabled */
#define PG_ACCESSED  BIT(5)   /* clear for non-accessed */
#define PT_DIRTY     BIT(6)   /* clear for non-dirty */
#define PD_4MEGPAGES BIT(7)   /* clear for 4KB pages */
#define PT_ATTRIDX   BIT(7)   /* ?? */
#define PT_GLOBAL    BIT(8)   /* clear for non-global */
#define PG_USR0      BIT(9)   /* available for software use */
#define PG_USR1      BIT(10)  /* `` */
#define PG_USR2      BIT(11)  /* `` */

typedef uint32 *pgtab_t, *drtab_t; /* REMOVE */

extern uint32 kern_drtab[], kern_pgtab[];

static inline void invlpg(void *p)
{
	__asm__ __volatile__ ("invlpg %0" : : "m"(*(char *) p) : "memory");
}

static inline uint32 cr2_get()
{
	register uint32 cr2;
	__asm__ __volatile__ ("mov %%cr2, %0" : "=q"(cr2));
	return cr2;
}

static inline uint32 cr3_get()
{
	register uint32 cr3;
	__asm__ __volatile__ ("mov %%cr3, %0" : "=q"(cr3));
	return cr3;
}

static inline void cr3_set(addr_t val)
{
	__asm__ __volatile__ ("mov %0, %%cr3" : : "q"(val) : "memory");
}

#define PAGE_SHIFT 12

/* Status */
#define PAGE_PRESENT PG_PRESENT
#define PAGE_DIRTY PT_DIRTY
#define PAGE_ACCESSED PG_ACCESSED

/* Permissions */
#define PAGE_USER PG_USERMODE
#define PAGE_KERN 0
#define PAGE_READ 0
#define PAGE_WRITE PG_WRITABLE
#define PAGE_EXEC 0

#define PAGE_ENT_NULL 0x00000000
#define PAGE_ENT_INVALID 0xfffffffe

#endif /* IA32_PAGING_H */
