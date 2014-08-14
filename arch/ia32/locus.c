/*
  arch/ia32/locus.c
  Copyright (c) 2005 Luke McCarthy
*/

#include <locus.h>
#include <process.h>
#include <pools.h>
#include <memory.h>
#include <pagealloc.h>
#include <console.h>
#include <paging.h>
#include <lib/string.h>
#include <lib/round.h>
#include <lib/assert.h>
#include <lib/ptrcast.h>
#include <ia32/locus.h>
#include <ia32/interrupt.h>
#include <ia32/segment.h>
#include <ia32/eflags.h>

struct kern_frame {
	struct pushad regs;
	uint32 num, errcode;
	struct iretd_kern iregs;
	void (*ret)();
	void *arg;
};

struct locus idle_locus = {
	.next = &idle_locus,
	.prev = &idle_locus,
	.status = LOCUS_STOPPED,
	.ctx = {
		.iregs = {
			.cs = KERN_CS
		},
		.cr3 = UVAL(TOPHYS(kern_drtab))
	},
	.ksp = NULL,
};

struct locus *kthread(void *ip, void *arg)
{
	void *sp;
	struct locus *l;
	struct kern_frame *f;

	l = alloc_locus();
	if (!l)
		return NULL;
	sp = kpage_alloc();
	if (!sp)
		goto err;
	f = ADDR(sp, PAGE_SIZE - sizeof(struct kern_frame));
	f->arg = arg;
	f->ret = kexit;
	f->iregs.eip = UVAL(ip);
	f->iregs.cs = KERN_CS;
	f->iregs.eflags = EFL_NULL | EFL_IF;
//	l->ctx.cr3 = UVAL(TOPHYS(kern_drtab));
	l->ctx.regs.esp = UVAL(f);
	l->ctx.iregs.cs = KERN_CS;
	l->status = LOCUS_READY;
	l->ksp = sp;
	locus_put(l);
	return l;
err:
	free(l);
	return NULL;
}

void kyield()
{
	__asm__ ("int $32");
}

void kexit()
{
	locus_stop(locus_current);
	kyield();
	while (1)
		__asm__ ("hlt");
}

struct locus *locus_create(struct process *ps, addr_t ip)
{
	struct locus *l;

	l = alloc_locus();
	if (!l)
		return NULL;
	memset(&l->ctx.regs, 0, sizeof(l->ctx.regs));
	l->ctx.iregs.eip = ip;
	l->ctx.iregs.cs = USER_CS | 3;
	l->ctx.iregs.eflags = EFL_NULL | EFL_IF;
	l->ctx.iregs.esp = 0;
	l->ctx.iregs.ss = USER_DS | 3;
	l->ctx.cr3 = ktophys(ps->dir);
	l->ksp = NULL;
	l->status = LOCUS_READY; //LOCUS_STOPPED;
	locus_put(l);
	return l;
}
