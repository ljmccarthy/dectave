/*
  include/string.h
  Copyright (c) 2005 Luke McCarthy

  String operations.
*/

#ifndef STRING_H
#define STRING_H

#include <macros.h>
#include <types.h>

extern PURE size_t strlen(const char *str);
extern void memset(void *dst, char ptn, size_t size);
extern void memcpy(void *dst, void *src, size_t size);

#endif /* STRING_H */
