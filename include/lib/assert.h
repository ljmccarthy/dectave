/*
  include/assert.h
  Copyright (c) 2005 Luke McCarthy

  Debug assertions.
*/

#ifndef ASSERT_H
#define ASSERT_H

#include <macros.h>

extern NORETURN void assert_fail
(const char *file, int line, const char *func, const char *expr);

#define ASSERT(e) do {                                          \
	if (!(e))                                               \
		assert_fail(__FILE__, __LINE__, __func__, #e);  \
} while (0)

#endif /* ASSERT_H */
