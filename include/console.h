/*
  include/console.h
  Copyright (c) 2005 Luke McCarthy

  Generic debug console interface.
*/

#ifndef CONSOLE_H
#define CONSOLE_H

#include <types.h>
#include <lib/ptrcast.h>

extern void cls();
extern void cursor_on(), cursor_off();
extern void nl(), sp(), bksp();
extern void putchar(char ch), rawputchar(char ch);

extern void print(const char *str);
extern void print_mem(const char *str, uint n);
extern void print_mem_raw(const char *str, uint n);

extern void print_int(int32 n, uint base);
extern void print_uint(uint32 n, uint base);
extern void print_hex(uint32 n);
extern void print_deci(int32 n);

extern void print_int_right(int32 n, uint base, char ch, uint off);
extern void print_int_left(int32 n, uint base, char ch, uint off);
extern void print_uint_right(uint32 n, uint base, char ch, uint off);
extern void print_uint_left(uint32 n, uint base, char ch, uint off);

extern void _print_ptr(uint32 p);
#define print_ptr(p) _print_ptr(UVAL(p))

extern void print_kbytes(size_t n);
extern void print_mbytes(size_t n);
extern void print_gbytes(size_t n);
extern void print_size(size_t n);

extern void dump(void *p);

extern void waitkey();

#endif /* CONSOLE_H */
