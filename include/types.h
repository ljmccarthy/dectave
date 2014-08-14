/*
  include/types.h
  Copyright (c) 2005 Luke McCarthy

  Too many types!
*/

#ifndef TYPES_H
#define TYPES_H

#include <config.h>

typedef _Bool bool;

#define FALSE ((bool) 0)
#define TRUE  ((bool) 1)

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;
typedef long long llong;
typedef unsigned long long ullong;

typedef signed char int8;
typedef signed short int int16;

typedef unsigned char uint8;
typedef unsigned short int uint16;

#if !defined BITS_PER_WORD
	#error "Please define BITS_PER_WORD in $__ARCH__/config.h"

#elif BITS_PER_WORD == 32

typedef signed int int32;
typedef signed long long int int64;

typedef unsigned int uint32;
typedef unsigned long long int uint64;

typedef uint32 size_t;
typedef uint32 addr_t;

typedef int32 intptr;
typedef uint32 uintptr;

#else
	#error "Please edit include/types.h for your arch's BITS_PER_WORD"

#endif /* BITS_PER_WORD */

#endif /* TYPES_H */
