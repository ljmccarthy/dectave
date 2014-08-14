/*
  arch/ia32/console.c
  Copyright (c) 2005 Luke McCarthy

  EGA 80x25 text console.
*/

#include <console.h>
#include <types.h>
#include <memory.h>
#include <lib/ptrcast.h>
#include <ia32/io.h>

#define W  80
#define H  25
#define XO 1
#define YO 1
#define XE (W-2)
#define YE (H-2)

static volatile uint x = XO, y = YO;

#define COLOUR 0x1f

#define VMEM ((uint16*)TOVIRT(0xb8000))
#define VMEM_COL(n) PTR(VMEM+W*(n))
#define VMEM_END VMEM_COL(H)

#define PUT(ch) do {                      \
	VMEM[y*W+x] = ((ch)|(COLOUR<<8)); \
} while (0)

static void scroll()
{
	volatile uint16
		*p = VMEM_COL(YO),
		*q = VMEM_COL(YO+1),
		*e = VMEM_COL(YE+1);

	while (q < e)
		*p++ = *q++;
	p++, e--;
	while (p < e)
		*p++ = COLOUR<<8 | ' ';
}

void cls()
{
	volatile uint32
		*p = VMEM_COL(0),
		*e = VMEM_END;

	while (p < e)
		*p++ = (COLOUR<<8) | (COLOUR<<24);
	x = XO;
	y = YO;
}

static void move()
{
	if (x == XE) {
		x = XO;
		if (y == YE)
			scroll();
		else
			y++;
	}
	else
		x++;
}

void nl()
{
	x = XO;
	if (y == YE)
		scroll();
	else
  		y++;
}

void bksp()
{
	if (x > XO)
		x--;
	else if (y > YO) {
		x = XE;
		y--;
	}
	PUT(' ');
}

void rawputchar(char ch)
{
	PUT(ch);
	move();
}

void putchar(char ch)
{
	switch (ch) {
		case '\n':
		case '\r':
			nl();
			break;
		case '\b':
			bksp();
			break;
		default:
			if (ch >= ' ') {
				PUT(ch);
				move();
			}
			break;
	}
}

void waitkey()
{
	while (!(in8(0x64) & 1) || in8(0x60) & 0x80)
		;
}
