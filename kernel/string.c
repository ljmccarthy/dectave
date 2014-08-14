/*
  kernel/string.c
  Copyright (c) 2005 Luke McCarthy

  String operations.
*/

#include <lib/string.h>
#include <lib/ptrcast.h>

size_t strlen(const char *str)
{
	const char *p = str;

	while (*p++)
		;
	return p - str - 1;
}

void memset(void *dst, char ptn, size_t size)
{
	char *di = dst;
	char *de = di + size;

	while (di < de)
		*di++ = ptn;
}

void memcpy(void *dst, void *src, size_t size)
{
	char *di = dst, *si = src;
	char *se = si + size;

	while (si < se)
		*di++ = *si++;
}
