/* ----- verify, derived from supercop/crypto_verify/try.c */

/*
derived from djb work from lib25519/libntruprime
mj modifications:
- rename files to test-crypto.c and _crypto_<>.<>.inc
- fix compiler warnings
- include crypto.h
- use less rounds for valgrind test
- reformat using clang-format
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "crypto.h"

static unsigned char *test_verify_16_x;
static unsigned char *test_verify_16_y;

static void test_verify_16_check(void) {
    unsigned char *x = test_verify_16_x;
    unsigned char *y = test_verify_16_y;
    int r;

    secret(x, 16);
    secret(y, 16);
    r = crypto_verify_16(x, y);
    public(x, 16);
    public(y, 16);
    public(&r, sizeof r);

    if (r == 0) {
        if (memcmp(x, y, 16)) fail("failure: different strings pass verify\n");
    }
    else if (r == -1) {
        if (!memcmp(x, y, 16)) fail("failure: equal strings fail verify\n");
    }
    else { fail("failure: weird return value\n"); }
}

void test_verify_16_impl(long long impl) {
    unsigned char *x = test_verify_16_x;
    unsigned char *y = test_verify_16_y;

    if (targetn && atol(targetn) != impl) return;

    randombytes(x, 16);
    randombytes(y, 16);
    test_verify_16_check();
    memcpy(y, x, 16);
    test_verify_16_check();
    y[myrandom() % 16] = myrandom();
    test_verify_16_check();
    y[myrandom() % 16] = myrandom();
    test_verify_16_check();
    y[myrandom() % 16] = myrandom();
    test_verify_16_check();
}

static void test_verify_16(void) {
    if (targeto && strcmp(targeto, "verify")) return;
    if (targetp && strcmp(targetp, "16")) return;

    test_verify_16_x = callocplus(16);
    test_verify_16_y = callocplus(16);

    for (long long offset = 0; offset < 2; ++offset) {
        if (targetoffset && atol(targetoffset) != offset) continue;
        if (offset && valgrind) break;
        printf("verify_16 offset %lld\n", offset);
        forked(test_verify_16_impl, -1);
        ++test_verify_16_x;
        ++test_verify_16_y;
    }
}
