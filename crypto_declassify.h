#ifndef crypto_declassify_h
#define crypto_declassify_h

#include "hasvalgrind.h"
#ifdef HASVALGRIND

#include <valgrind/memcheck.h>

#define crypto_declassify_uses_valgrind 1

static void crypto_declassify(void *xv, long long xlen) {
    VALGRIND_MAKE_MEM_DEFINED(xv, xlen);
}

#else

#define crypto_declassify_uses_valgrind 0

static void crypto_declassify(void *xv, long long xlen) {
    (void) xv;
    (void) xlen;
}

#endif
#endif
