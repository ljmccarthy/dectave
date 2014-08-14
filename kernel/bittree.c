/*
  kernel/bittree.c
  Copyright (c) 2005 Luke McCarthy

  Duotrigesimal Bit Tree

  The bit tree can find any set bit in a bitmap of 1,048,576 bits
  by examining only 128 bits (four words) i.e. log32(1048576) = 4.

  A set bit (1) marks an available slot. A clear bit (0) marks an allocated
  slot. This may seem unintuitive; however it allows searching with the
  'bsf' instruction without having to do unnecessary bit inversions.

  The lowest level, b4, is where each bit marks free or allocated slot.
  If bit j in b3[i] is set (marked free), there must be at least one set bit
  in b4[i*32 + j], otherwise they are all clear (marked allocated).

  A pleasant side-effect of the algorithm is that slots are always allocated
  from the lowest available slot first.
*/

#include <bittree.h>
#include <lib/divide.h>
#include ARCH(bitmap.h)

void btr_markfree(struct bittree *btr, uint32 off, uint32 end)
{
	uint32 i;

	btr->free += end - off;

	for (i = off; i < end; i++)
		set(btr->b4, i);

	off /= 32;
	end = udivup(end, 32);
	for (i = off; i < end; i++)
		set(btr->b3, i);

	off /= 32;
	end = udivup(end, 32);
	for (i = off; i < end; i++)
		set(btr->b2, i);

	off /= 32;
	end = udivup(end, 32);
	for (i = off; i < end; i++)
		set(&btr->b1, i);
}

uint32 btr_alloc(struct bittree *btr)
{
	uint32 i, j, k, l;

	if (!btr->b1)
		return 0;

	btr->free--;

	i = ffs(btr->b1);
	j = ffs(btr->b2[i]) + i * 32;
	k = ffs(btr->b3[j]) + j * 32;
	l = ffs(btr->b4[k]) + k * 32;

	clr(btr->b4, l); if (btr->b4[k]) return l;
	clr(btr->b3, k); if (btr->b3[j]) return l;
	clr(btr->b2, j); if (btr->b2[i]) return l;
	clr(&btr->b1, i); return l;
}

void btr_free(struct bittree *btr, uint32 i)
{
	btr->free++;
	set(btr->b4, i); i /= 32;
	set(btr->b3, i); i /= 32;
	set(btr->b2, i); i /= 32;
	set(&btr->b1, i);
}
