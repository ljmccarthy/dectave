/*
  kernel/console.c
  Copyright (c) 2005 Luke McCarthy

  Generic console procedures.
*/

#include <console.h>
#include <types.h>
#include <memory.h>
#include <lib/string.h>
#include <lib/clip.h>

#define MAX_UINT32_BUF 33
#define MAX_INT32_BUF (MAX_UINT32_BUF+1)

void sp()
{
	putchar(' ');
}

void print(const char *str)
{
	while (*str)
		putchar(*str++);
}

void print_mem(const char *str, uint n)
{
	while (n--)
		putchar(*str++);
}

void print_mem_raw(const char *str, uint n)
{
	while (n--)
		rawputchar(*str++);
}

static inline char todigit(uint n)
{
	 return n + (n < 10 ? '0' : ('A'-10));
}

static char *_uint32_to_str(uint32 n, uint base, char buf[MAX_UINT32_BUF])
{
	int i = MAX_UINT32_BUF;

	buf[--i] = '\0';
	do
		buf[--i] = todigit(n % base);
	while ((n /= base) > 0);
	return &buf[i];
}

static char *_int32_to_str(int32 n, uint base, char buf[MAX_INT32_BUF])
{
	int sign = n < 0;
	char *p = _uint32_to_str(sign ? -n : n, base, &buf[1]);

	if (sign)
		*--p = '-';
	return p;
}

static inline char *uint32_to_str(
	uint32 n, uint base, char buf[MAX_UINT32_BUF])
{
	return _uint32_to_str(n, base-1 < 36 ? base : 10, buf);
}

static inline char *int32_to_str(
	int32 n, uint base, char buf[MAX_INT32_BUF])
{
	return _int32_to_str(n, base-1 < 36 ? base : 10, buf);
}

void print_int(int32 n, uint base)
{
	char buf[MAX_INT32_BUF];

	print(int32_to_str(n, base, buf));
}

void print_uint(uint32 n, uint base)
{
	char buf[MAX_UINT32_BUF];

	print(uint32_to_str(n, base, buf));
}

void print_hex(uint32 n)
{
	print_uint(n, 0x10);
}

void print_deci(int32 n)
{
	print_int(n, 10);
}

static void print_left(char *str, uint len, uint off, char ch)
{
	int fill = off - len;

	print_mem_raw(str, umin(off, len));
	while (fill--)
		putchar(ch);
}

static void print_right(char *str, uint len, uint off, char ch)
{
	int fill = off - len;

	while (fill--)
		putchar(ch);
	print_mem_raw(str, umin(off, len));
}

void print_uint_left(uint32 n, uint base, char ch, uint off)
{
	char *str, buf[MAX_UINT32_BUF];

	str = uint32_to_str(n, base, buf);
	print_left(str, strlen(str), off, ch);
}

void print_uint_right(uint32 n, uint base, char ch, uint off)
{
	char *str, buf[MAX_UINT32_BUF];

	str = uint32_to_str(n, base, buf);
	print_right(str, strlen(str), off, ch);
}

void print_int_left(int32 n, uint base, char ch, uint off)
{
	char *str, buf[MAX_INT32_BUF];

	str = int32_to_str(n, base, buf);
	print_left(str, strlen(str), off, ch);
}

void print_int_right(int32 n, uint base, char ch, uint off)
{
	char *str, buf[MAX_INT32_BUF];

	str = int32_to_str(n, base, buf);
	print_right(str, strlen(str), off, ch);
}

void _print_ptr(uint32 p)
{
	putchar('$');
	print_uint_right(p, 0x10, '0', 8);
}

#define KILO 1024
#define MEGA (1024*KILO)
#define GIGA (1024*MEGA)

#define T_funcname   print_kbytes
#define T_multi      KILO
#define T_multiname  " KiB"
#include "printbytes.ct"

#define T_funcname   print_mbytes
#define T_multi      MEGA
#define T_multiname  " MiB"
#include "printbytes.ct"

#define T_funcname   print_gbytes
#define T_multi      GIGA
#define T_multiname  " GiB"
#include "printbytes.ct"

void print_size(size_t n)
{
	if (n < KILO) {
		print_uint(n, 10);
		print(" B");
	}
	else if (n < MEGA)
		print_kbytes(n);
	else if (n < GIGA)
		print_mbytes(n);
	else
		print_gbytes(n);
}

/*
  hex dump 256 bytes
*/
void dump(void *p)
{
	uint i;
	uchar *cp = p, *ep = (uchar *) p + 256;

	while (cp < ep) {
		print_ptr(cp);
		print("  ");
		for (i = 0; i < 16; i++) {
			print_uint_left(cp[i], 0x10, '0', 2);
			sp();
		}
		print("  ");
		for (i = 0; i < 16; i++)
			rawputchar(*cp++);
		nl();
	}
}
