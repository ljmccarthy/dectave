#ifndef BITTREE_H
#define BITTREE_H

#include <types.h>

struct bittree {
	uint32 free;
	uint32
		 b1,  /* [0x1]            */
		*b2,  /* [0x20] or less   */
		*b3,  /* [0x400] or less  */
		*b4;  /* [0x8000] or less */
};

extern void btr_markfree(struct bittree *btr, uint32 off, uint32 end);

extern uint32 btr_alloc(struct bittree *btr);

extern void btr_free(struct bittree *btr, uint32 i);

#endif /* BITTREE_H */
