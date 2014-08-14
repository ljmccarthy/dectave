/*
  include/ia32/segment.h
  Copyright (c) 2005 Luke McCarthy

  Segmentation (selectors).
*/

#ifndef IA32_SEGMENT_H
#define IA32_SEGMENT_H

#define SELECTOR(n) ((n)<<3)

#define KERN_CS SELECTOR(1)
#define KERN_DS SELECTOR(2)
#define USER_CS SELECTOR(3)
#define USER_DS SELECTOR(4)
#define KERN_TSS SELECTOR(5)

#endif /* IA32_SEGMENT_H */
