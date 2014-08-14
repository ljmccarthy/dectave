/*
  arch/ia32/interrupt.c
  Copyright (c) 2005 Luke McCarthy

  Interrupt handling.

  Each interrupt handler may return NULL or a Locus pointer.
  If NULL is returned, control is returned to the interrupted
  locus. Othewise, control is switched to the returned locus.
*/

#include <config.h>
#include <console.h>
#include <macros.h>
#include <locus.h>
#include <halt.h>
#include <ia32/interrupt.h>
#include <ia32/io.h>
#include <ia32/debug.h>

uint32 lock_level = 0;

struct locus *irq_timer(UNUSED struct trap *frame)
{
	static volatile uint32 tick = 0;

	tick++;
	return tick % QUANTUM ? NULL : locus_next();
}

struct locus *irq_keyb(UNUSED struct trap *frame)
{
	uint8 key;
	static uint8 cad = 0, lastkey = 0;

	key = in8(0x60);
	print_hex(key); sp();

	if (lastkey == 0xE0) switch (key) {
		case 0x53: cad |= 4; break;
		case 0xD3: cad &=~4; break;
	}
	else switch (key) {
		case 0x1D: cad |= 1; break;
		case 0x9D: cad &=~1; break;
		case 0x38: cad |= 2; break;
		case 0xB8: cad &=~2; break;
	}
	if (cad == 7)
		reboot();

	lastkey = key;

	return NULL;
}

struct locus *irq_null(struct trap *frame)
{
	print_trap(frame);
	halt();
	return NULL;
}

typedef struct locus *(*trap_handler_t)(struct trap *);

static trap_handler_t irqs[NUM_IRQS] = {
	irq_timer, irq_keyb, irq_null, irq_null,
	irq_null,  irq_null, irq_null, irq_null,
	irq_null,  irq_null, irq_null, irq_null,
	irq_null,  irq_null, irq_null, irq_null,
};

static trap_handler_t traps[NUM_TRAPS] = {
	irq_null, irq_null, irq_null, irq_null,
	irq_null, irq_null, irq_null, irq_null,
	irq_null, irq_null, irq_null, irq_null,
	irq_null, irq_null, irq_null, irq_null,
	irq_null, irq_null, irq_null, irq_null,
};

struct locus *irq_entry(struct trap frame)
{
	lock_level++;
	struct locus *l = irqs[frame.num](&frame);
	lock_level--;
	return l;
}

struct locus *trap_entry(struct trap frame)
{
	if (lock_level == 0) {
		lock_level++;
		struct locus *l = traps[frame.num](&frame);
		lock_level--;
		return l;
	}
	else {
		print_trap(&frame);
		die("System halted.");
	}
}

struct locus *syscall_entry(UNUSED struct syscall frame)
{
	print("USER "); print_ptr(locus_current); nl();
	return NULL;
}
