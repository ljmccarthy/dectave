/*
  util/clip.h
  Copyright (c) 2005 Luke McCarthy

  Integer clipping.
*/

#ifndef UTIL_CLIP_H
#define UTIL_CLIP_H

#include <macros.h>
#include <types.h>

PURE static inline int min(int a, int b)
{
	return a < b ? a : b;
}

PURE static inline int max(int a, int b)
{
	return a < b ? b : a;
}

PURE static inline int clip(int a, int min, int max)
{
	return a < min ? min : (a > max ? max : a);
}

PURE static inline int within(int a, int min, int max)
{
	return a >= min && a <= max;
}

PURE static inline uint umin(uint a, uint b)
{
	return a < b ? a : b;
}

PURE static inline uint umax(uint a, uint b)
{
	return a < b ? b : a;
}

PURE static inline uint uclip(uint a, uint min, uint max)
{
	return a < min ? min : (a > max ? max : a);
}

PURE static inline int uwithin(uint a, uint min, uint max)
{
	return a >= min && a <= max;
}

#endif /* UTIL_CLIP_H */
