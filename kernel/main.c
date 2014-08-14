/*
  kernel/main.c
  Copyright (c) 2005 Luke McCarthy

  Kernel initialisation.
*/

#include <console.h>
#include <memory.h>
#include <types.h>
#include <locus.h>
#include <interrupt.h>

#if 0

#define NR_LOCUS 2000
#define NR_LUSER 1000

#if NR_LOCUS

static struct loopy_arg {
	uint num, delay;
}
loopy_arg[3] = {
	{ .num=25, .delay=600000 },
	{ .num=50, .delay=200000 },
	{ .num=30, .delay=300000 },
};

#define DELAY(n) do {                   \
	for (uint i = 0; i < (n); i++)  \
		;                       \
} while (0)

static void loopy(struct loopy_arg *arg)
{
#if 0
	for (uint i = 0; i < arg->num; i++) {
#else
	while (1) {
#endif
		lock();
		//print(arg->msg); sp(); print_deci(i); nl();
		print("KERN "); print_ptr(locus_current); nl();
		unlock();
		kyield();
		DELAY(arg->delay);
	}
}

#endif /* NR_LOCUS */

#include <halt.h>
#include <process.h>
#include <lib/assert.h>
#include <lib/string.h>


static void init_tests()
{
#if NR_LOCUS
	for (uint i = 0; i < NR_LOCUS; i++)
		kthread(loopy, &loopy_arg[i%3]);
#endif

#if NR_LUSER
	extern char _user[], _euser[];
	addr_t ip = 0x500000;

	for (uint i = 0; i < NR_LUSER; i++) {
		struct process *ps = ps_create();
		ASSERT(ps);
		ps_commit(ps, ip, PAGE_ALIGN(_euser - _user));
		ps_select(ps);
		memcpy((void *) ip, _user, _euser - _user);
		locus_create(ps, ip);
	}
#endif
}

#endif /* 0 */

static void print_boot_info()
{
	print("Dectave 0.1 Sept 2005 by Luke McCarthy\n\n");
	print("* Kernel image is ");
	print_size(_ebss-_img); nl();
}

void main()
{
	extern void
		init_memory(),
		init_pools(),
		init_devices();

	cls();
	print_boot_info();
	init_memory();
	init_pools();
	init_devices();
//	init_tests();
}
