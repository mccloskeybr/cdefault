#ifndef CDEFAULT_ASSERT_H
#define CDEFAULT_ASSERT_H

#include <assert.h>

#ifdef DEBUG
#define ASSERT(exp) assert(exp)
#else
#define ASSERT(exp) (exp)
#endif

#ifdef DEBUG
#define STATIC_ASSERT(exp, msg) static_assert((exp), msg)
#else
#define STATIC_ASSERT(exp)
#endif

#define UNIMPLEMENTED() ASSERT(false)
#define UNREACHABLE() ASSERT(false)
#define TODO() ASSERT(false)

#endif
