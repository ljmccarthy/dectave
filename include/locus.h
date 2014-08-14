/*
  include/locus.h
  Copyright (c) 2005 Luke McCarthy

  Locus (fancy name for thread) interface.
*/

#ifndef LOCUS_H
#define LOCUS_H

#include <macros.h>
#include <types.h>
#include ARCH(locus.h)  /* for Context */

struct process;

/*
  Context-switching assembly code will assume that the context
  structure containing the register context is at the top of the
  structure. This is for convenience. Best not to move it around.
*/
struct locus {
	struct thread_context ctx;
	struct locus *next, *prev;
	enum {
		LOCUS_READY,
		LOCUS_WAITING,
		LOCUS_STOPPED,
		LOCUS_EXITED,
	} status;
	void *ksp;
};

extern uint locus_count;
extern struct locus *locus_current;
extern struct locus idle_locus;

extern struct locus *locus_put(struct locus *l);
extern struct locus *locus_next();
extern struct locus *locus_run(struct locus *l);
extern void locus_stop(struct locus *l);

extern struct locus *kthread(void *ip, void *arg);
extern NORETURN void kexit();
extern void kyield();

/* TENTATIVE */
extern struct locus *locus_create(struct process *ps, addr_t ip);

#endif /* LOCUS_H */
