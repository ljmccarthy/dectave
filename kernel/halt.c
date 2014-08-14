/*
  kernel/halt.c
  Copyright (c) 2005 Luke McCarthy

  Die function (not German).
*/

#include <console.h>
#include <halt.h>
#include <interrupt.h>
#include <lib/assert.h>

void die(char *msg)
{
	lock();
	print("\nUnrecoverable Error: "); print(msg);
	halt();
}

void assert_fail(const char *file, int line, const char *func, const char *expr)
{
	lock();
	print("\nAssertion Failed\n  In ");
	print(file); putchar('/');
	print(func); print("() line ");
	print_deci(line); print("\n    ");
	print(expr); nl();
	die("System halted.");
}
