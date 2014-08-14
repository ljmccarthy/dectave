/*
  include/ia32/multiboot.h
  Copyright (c) 2005 Luke McCarthy

  GRUB multiboot.
*/

#ifndef IA32_MULTIBOOT_H
#define IA32_MULTIBOOT_H

#include <types.h>

struct boot_memory_info {
	uint32 size;
	uint32 base_addr_lo;
	uint32 base_addr_hi;
	uint32 length_lo;
	uint32 length_hi;
	uint32 type;
};

struct boot_module_info {
	uint32 start;
	uint32 end;
	uint32 string;
	uint32 reserved;
};

struct boot_aout_info {
	uint32 tabsize;
	uint32 strsize;
	uint32 addr;
	uint32 _;
};

struct boot_elf_info {
	uint32 num;
	uint32 size;
	uint32 addr;
	uint32 shndx;
};

struct boot_info {
	uint32 flags;
	uint32 mem_lower;
	uint32 mem_upper;
	uint32 boot_dev;
	uint32 cmdline;
	uint32 mod_count;
	struct boot_module_info *mods;
	union {
		struct boot_aout_info aout;
		struct boot_elf_info elf;
	} hdr;
	uint32 mmap_length;
	uint32 mmap_addr;
};

extern struct boot_info *mbinf;

#define foreach_memmap(m)                                              \
	for (                                                          \
		struct boot_memory_info *m =                           \
			(struct boot_memory_info *) mbinf->mmap_addr;  \
		(uint32) m < mbinf->mmap_addr + mbinf->mmap_length;    \
		m = (struct boot_memory_info *)                        \
			((uint32) m + m->size + sizeof(m->size)))

#endif /* IA32_MULTIBOOT_H */
