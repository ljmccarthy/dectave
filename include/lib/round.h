/*
  util/round.h
  Copyright (c) 2005 Luke McCarthy

  Integer rounding.
*/

#ifndef UTIL_ROUND_H
#define UTIL_ROUND_H

#include <types.h>
#include <macros.h>

PURE static inline intptr ifloordiff(intptr a, intptr b)
{
	return a % b;
}

PURE static inline intptr iceildiff(intptr a, intptr b)
{
	return (((a - 1) % b) - b) + 1;
}

PURE static inline intptr irounddiff(intptr a, intptr b)
{
	return (((a - b/2) % b) - b) + b/2;
}

PURE static inline intptr ifloor(intptr a, intptr b)
{
	return a - ifloordiff(a, b);
}

PURE static inline intptr iceil(intptr a, intptr b)
{
	return a - iceildiff(a, b);
}

PURE static inline intptr iround(intptr a, intptr b)
{
	return a - irounddiff(a, b);
}

PURE static inline uintptr ufloordiff(uintptr a, uintptr b)
{
	return a % b;
}

PURE static inline uintptr uceildiff(uintptr a, uintptr b)
{
	return (((a - 1) % b) - b) + 1;
}

PURE static inline uintptr urounddiff(uintptr a, uintptr b)
{
	return (((a - b/2) % b) - b) + b/2;
}

PURE static inline uintptr ufloor(uintptr a, uintptr b)
{
	return a - ufloordiff(a, b);
}

PURE static inline uintptr uceil(uintptr a, uintptr b)
{
	return a - uceildiff(a, b);
}

PURE static inline uintptr uround(uintptr a, uintptr b)
{
	return a - urounddiff(a, b);
}

#endif /* UTIL_ROUND_H */
