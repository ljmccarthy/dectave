/*
  util/macros.h
  Copyright (c) 2005 Luke McCarthy
*/

#ifndef UTIL_MACROS_H
#define UTIL_MACROS_H

#ifndef NULL
  #define NULL ((void*)0)
#endif
#if 0
  #define INVALID ((void*)~0)
#endif

/* Argument annotations. */

#define IN
#define OUT
#define INOUT

/* Attributes. */

#if defined(__GNUC__)
  #define PACKED __attribute__((packed))
  #define PURE __attribute__((pure))
  #define NORETURN __attribute__((noreturn))
  #define UNUSED __attribute__((unused))
  #define INIT __attribute__((constructor))
  #define FINI __attribute__((destructor))
  #ifndef FASTCALL
    #define FASTCALL __attribute__((regparm(3)))
  #endif
  #define EXPORT __attribute__((visibility("default")))
#elif defined(_MSC_VER)
  #define EXPORT __declspec(dllexport)
#endif

/* Inlining support. */

#if defined(__GNUC__) && __STDC_VERSION__ < 199901L
  #define inline __inline__
#elif defined(_MSC_VER)
  #define inline __inline
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ < 199901L
  #define inline
#endif

/* Bits and bytes and bobs. */

#define BIT(n) (1 << (n))
#define NELEMS(a) (sizeof(a) / sizeof((a)[0]))

/* For defining block macros. */

#define DO do {
#define END ; } while (0)

#endif /* UTIL_MACROS_H */
