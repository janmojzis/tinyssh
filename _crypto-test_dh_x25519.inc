/* ----- dh/x25519, derived from supercop/crypto_dh/try.c */

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
#include "crypto_dh_x25519.h"

#define fail ((ok = 0), printf)
static const char *dh_x25519_checksums[] = {
    "2c8a73ec86d5d4c4bc838f49cfd78c87b60b534ae6fff59ce3bea0c32cdc1450",
    "b09016b3a1371786b46a183085133338159e623c5eb9cbc5eaa4f8b62d6c5aea",
};

static int (*crypto_dh_keypair)(unsigned char *, unsigned char *);
static int (*crypto_dh)(unsigned char *, const unsigned char *,
                        const unsigned char *);
#define crypto_dh_SECRETKEYBYTES crypto_dh_x25519_SECRETKEYBYTES
#define crypto_dh_PUBLICKEYBYTES crypto_dh_x25519_PUBLICKEYBYTES
#define crypto_dh_BYTES crypto_dh_x25519_BYTES

static void *storage_dh_x25519_a;
static unsigned char *test_dh_x25519_a;
static void *storage_dh_x25519_b;
static unsigned char *test_dh_x25519_b;
static void *storage_dh_x25519_c;
static unsigned char *test_dh_x25519_c;
static void *storage_dh_x25519_d;
static unsigned char *test_dh_x25519_d;
static void *storage_dh_x25519_e;
static unsigned char *test_dh_x25519_e;
static void *storage_dh_x25519_f;
static unsigned char *test_dh_x25519_f;
static void *storage_dh_x25519_a2;
static unsigned char *test_dh_x25519_a2;
static void *storage_dh_x25519_b2;
static unsigned char *test_dh_x25519_b2;
static void *storage_dh_x25519_c2;
static unsigned char *test_dh_x25519_c2;
static void *storage_dh_x25519_d2;
static unsigned char *test_dh_x25519_d2;
static void *storage_dh_x25519_e2;
static unsigned char *test_dh_x25519_e2;
static void *storage_dh_x25519_f2;
static unsigned char *test_dh_x25519_f2;

static void test_dh_x25519_impl(long long impl) {
    unsigned char *a = test_dh_x25519_a;
    unsigned char *b = test_dh_x25519_b;
    unsigned char *c = test_dh_x25519_c;
    unsigned char *d = test_dh_x25519_d;
    unsigned char *e = test_dh_x25519_e;
    unsigned char *f = test_dh_x25519_f;
    unsigned char *a2 = test_dh_x25519_a2;
    unsigned char *b2 = test_dh_x25519_b2;
    unsigned char *c2 = test_dh_x25519_c2;
    unsigned char *d2 = test_dh_x25519_d2;
    unsigned char *e2 = test_dh_x25519_e2;
    unsigned char *f2 = test_dh_x25519_f2;
    long long alen = crypto_dh_SECRETKEYBYTES;
    long long blen = crypto_dh_SECRETKEYBYTES;
    long long clen = crypto_dh_PUBLICKEYBYTES;
    long long dlen = crypto_dh_PUBLICKEYBYTES;
    long long elen = crypto_dh_BYTES;
    long long flen = crypto_dh_BYTES;

    if (targetn && atol(targetn) != impl) return;
    crypto_dh_keypair = crypto_dh_x25519_keypair;
    crypto_dh = crypto_dh_x25519;
    for (long long checksumbig = 0; checksumbig < 2; ++checksumbig) {
        long long loops = checksumbig ? 512 : 64;

        if (checksumbig && valgrind) break;
        checksum_clear();

        for (long long loop = 0; loop < loops; ++loop) {

            output_prepare(c2, c, clen);
            output_prepare(a2, a, alen);
            crypto_dh_keypair(c, a);
            public(c, clen);
            public(a, alen);
            checksum(c, clen);
            checksum(a, alen);
            output_compare(c2, c, clen, "crypto_dh_keypair");
            output_compare(a2, a, alen, "crypto_dh_keypair");
            output_prepare(d2, d, dlen);
            output_prepare(b2, b, blen);
            crypto_dh_keypair(d, b);
            public(d, dlen);
            public(b, blen);
            checksum(d, dlen);
            checksum(b, blen);
            output_compare(d2, d, dlen, "crypto_dh_keypair");
            output_compare(b2, b, blen, "crypto_dh_keypair");
            output_prepare(e2, e, elen);
            memcpy(d2, d, dlen);
            double_canary(d2, d, dlen);
            memcpy(a2, a, alen);
            double_canary(a2, a, alen);
            secret(d, dlen);
            secret(a, alen);
            crypto_dh(e, d, a);
            public(d, dlen);
            public(a, alen);
            public(e, elen);
            checksum(e, elen);
            output_compare(e2, e, elen, "crypto_dh");
            input_compare(d2, d, dlen, "crypto_dh");
            input_compare(a2, a, alen, "crypto_dh");

            double_canary(e2, e, elen);
            double_canary(d2, d, dlen);
            double_canary(a2, a, alen);
            secret(d2, dlen);
            secret(a2, alen);
            crypto_dh(e2, d2, a2);
            public(d2, dlen);
            public(a2, alen);
            public(e2, elen);
            if (memcmp(e2, e, elen) != 0)
                fail("failure: crypto_dh is nondeterministic\n");

            double_canary(e2, e, elen);
            double_canary(d2, d, dlen);
            double_canary(a2, a, alen);
            secret(d2, dlen);
            secret(a, alen);
            crypto_dh(d2, d2, a);
            public(d2, elen);
            public(a, alen);
            if (memcmp(d2, e, elen) != 0)
                fail("failure: crypto_dh does not handle d=e overlap\n");
            memcpy(d2, d, dlen);
            secret(d, dlen);
            secret(a2, alen);
            crypto_dh(a2, d, a2);
            public(a2, elen);
            public(d, dlen);
            if (memcmp(a2, e, elen) != 0)
                fail("failure: crypto_dh does not handle a=e overlap\n");
            memcpy(a2, a, alen);
            output_prepare(f2, f, flen);
            memcpy(c2, c, clen);
            double_canary(c2, c, clen);
            memcpy(b2, b, blen);
            double_canary(b2, b, blen);
            secret(c, clen);
            secret(b, blen);
            crypto_dh(f, c, b);
            public(c, clen);
            public(b, blen);
            public(f, flen);
            checksum(f, flen);
            output_compare(f2, f, flen, "crypto_dh");
            input_compare(c2, c, clen, "crypto_dh");
            input_compare(b2, b, blen, "crypto_dh");

            double_canary(f2, f, flen);
            double_canary(c2, c, clen);
            double_canary(b2, b, blen);
            secret(c2, clen);
            secret(b2, blen);
            crypto_dh(f2, c2, b2);
            public(c2, clen);
            public(b2, blen);
            public(f2, flen);
            if (memcmp(f2, f, flen) != 0)
                fail("failure: crypto_dh is nondeterministic\n");

            double_canary(f2, f, flen);
            double_canary(c2, c, clen);
            double_canary(b2, b, blen);
            secret(c2, clen);
            secret(b, blen);
            crypto_dh(c2, c2, b);
            public(c2, flen);
            public(b, blen);
            if (memcmp(c2, f, flen) != 0)
                fail("failure: crypto_dh does not handle c=f overlap\n");
            memcpy(c2, c, clen);
            secret(c, clen);
            secret(b2, blen);
            crypto_dh(b2, c, b2);
            public(b2, flen);
            public(c, clen);
            if (memcmp(b2, f, flen) != 0)
                fail("failure: crypto_dh does not handle b=f overlap\n");
            memcpy(b2, b, blen);

            if (memcmp(f, e, elen) != 0)
                fail("failure: crypto_dh not associative\n");
        }
        checksum_expected(dh_x25519_checksums[checksumbig]);
    }
}

void test_dh_x25519(void) {
    long long maxalloc = 0;
    if (targeto && strcmp(targeto, "dh")) return;
    if (targetp && strcmp(targetp, "x25519")) return;
    storage_dh_x25519_a = callocplus(
        crypto_dh_BYTES + crypto_dh_PUBLICKEYBYTES + crypto_dh_SECRETKEYBYTES);
    test_dh_x25519_a = aligned(storage_dh_x25519_a,
                               crypto_dh_BYTES + crypto_dh_PUBLICKEYBYTES +
                                   crypto_dh_SECRETKEYBYTES);
    if (crypto_dh_BYTES + crypto_dh_PUBLICKEYBYTES + crypto_dh_SECRETKEYBYTES >
        maxalloc)
        maxalloc = crypto_dh_BYTES + crypto_dh_PUBLICKEYBYTES +
                   crypto_dh_SECRETKEYBYTES;
    storage_dh_x25519_b = callocplus(
        crypto_dh_BYTES + crypto_dh_PUBLICKEYBYTES + crypto_dh_SECRETKEYBYTES);
    test_dh_x25519_b = aligned(storage_dh_x25519_b,
                               crypto_dh_BYTES + crypto_dh_PUBLICKEYBYTES +
                                   crypto_dh_SECRETKEYBYTES);
    if (crypto_dh_BYTES + crypto_dh_PUBLICKEYBYTES + crypto_dh_SECRETKEYBYTES >
        maxalloc)
        maxalloc = crypto_dh_BYTES + crypto_dh_PUBLICKEYBYTES +
                   crypto_dh_SECRETKEYBYTES;
    storage_dh_x25519_c = callocplus(
        crypto_dh_BYTES + crypto_dh_PUBLICKEYBYTES + crypto_dh_SECRETKEYBYTES);
    test_dh_x25519_c = aligned(storage_dh_x25519_c,
                               crypto_dh_BYTES + crypto_dh_PUBLICKEYBYTES +
                                   crypto_dh_SECRETKEYBYTES);
    if (crypto_dh_BYTES + crypto_dh_PUBLICKEYBYTES + crypto_dh_SECRETKEYBYTES >
        maxalloc)
        maxalloc = crypto_dh_BYTES + crypto_dh_PUBLICKEYBYTES +
                   crypto_dh_SECRETKEYBYTES;
    storage_dh_x25519_d = callocplus(
        crypto_dh_BYTES + crypto_dh_PUBLICKEYBYTES + crypto_dh_SECRETKEYBYTES);
    test_dh_x25519_d = aligned(storage_dh_x25519_d,
                               crypto_dh_BYTES + crypto_dh_PUBLICKEYBYTES +
                                   crypto_dh_SECRETKEYBYTES);
    if (crypto_dh_BYTES + crypto_dh_PUBLICKEYBYTES + crypto_dh_SECRETKEYBYTES >
        maxalloc)
        maxalloc = crypto_dh_BYTES + crypto_dh_PUBLICKEYBYTES +
                   crypto_dh_SECRETKEYBYTES;
    storage_dh_x25519_e = callocplus(
        crypto_dh_BYTES + crypto_dh_PUBLICKEYBYTES + crypto_dh_SECRETKEYBYTES);
    test_dh_x25519_e = aligned(storage_dh_x25519_e,
                               crypto_dh_BYTES + crypto_dh_PUBLICKEYBYTES +
                                   crypto_dh_SECRETKEYBYTES);
    if (crypto_dh_BYTES + crypto_dh_PUBLICKEYBYTES + crypto_dh_SECRETKEYBYTES >
        maxalloc)
        maxalloc = crypto_dh_BYTES + crypto_dh_PUBLICKEYBYTES +
                   crypto_dh_SECRETKEYBYTES;
    storage_dh_x25519_f = callocplus(
        crypto_dh_BYTES + crypto_dh_PUBLICKEYBYTES + crypto_dh_SECRETKEYBYTES);
    test_dh_x25519_f = aligned(storage_dh_x25519_f,
                               crypto_dh_BYTES + crypto_dh_PUBLICKEYBYTES +
                                   crypto_dh_SECRETKEYBYTES);
    if (crypto_dh_BYTES + crypto_dh_PUBLICKEYBYTES + crypto_dh_SECRETKEYBYTES >
        maxalloc)
        maxalloc = crypto_dh_BYTES + crypto_dh_PUBLICKEYBYTES +
                   crypto_dh_SECRETKEYBYTES;
    storage_dh_x25519_a2 = callocplus(maxalloc);
    test_dh_x25519_a2 = aligned(storage_dh_x25519_a2,
                                crypto_dh_BYTES + crypto_dh_PUBLICKEYBYTES +
                                    crypto_dh_SECRETKEYBYTES);
    storage_dh_x25519_b2 = callocplus(maxalloc);
    test_dh_x25519_b2 = aligned(storage_dh_x25519_b2,
                                crypto_dh_BYTES + crypto_dh_PUBLICKEYBYTES +
                                    crypto_dh_SECRETKEYBYTES);
    storage_dh_x25519_c2 = callocplus(maxalloc);
    test_dh_x25519_c2 = aligned(storage_dh_x25519_c2,
                                crypto_dh_BYTES + crypto_dh_PUBLICKEYBYTES +
                                    crypto_dh_SECRETKEYBYTES);
    storage_dh_x25519_d2 = callocplus(maxalloc);
    test_dh_x25519_d2 = aligned(storage_dh_x25519_d2,
                                crypto_dh_BYTES + crypto_dh_PUBLICKEYBYTES +
                                    crypto_dh_SECRETKEYBYTES);
    storage_dh_x25519_e2 = callocplus(maxalloc);
    test_dh_x25519_e2 = aligned(storage_dh_x25519_e2,
                                crypto_dh_BYTES + crypto_dh_PUBLICKEYBYTES +
                                    crypto_dh_SECRETKEYBYTES);
    storage_dh_x25519_f2 = callocplus(maxalloc);
    test_dh_x25519_f2 = aligned(storage_dh_x25519_f2,
                                crypto_dh_BYTES + crypto_dh_PUBLICKEYBYTES +
                                    crypto_dh_SECRETKEYBYTES);

    for (long long offset = 0; offset < 2; ++offset) {
        if (targetoffset && atol(targetoffset) != offset) continue;
        if (offset && valgrind) break;
        printf("dh_x25519 offset %lld\n", offset);
        forked(test_dh_x25519_impl, -1);
        ++test_dh_x25519_a;
        ++test_dh_x25519_b;
        ++test_dh_x25519_c;
        ++test_dh_x25519_d;
        ++test_dh_x25519_e;
        ++test_dh_x25519_f;
        ++test_dh_x25519_a2;
        ++test_dh_x25519_b2;
        ++test_dh_x25519_c2;
        ++test_dh_x25519_d2;
        ++test_dh_x25519_e2;
        ++test_dh_x25519_f2;
    }
    free(storage_dh_x25519_f2);
    free(storage_dh_x25519_e2);
    free(storage_dh_x25519_d2);
    free(storage_dh_x25519_c2);
    free(storage_dh_x25519_b2);
    free(storage_dh_x25519_a2);
    free(storage_dh_x25519_f);
    free(storage_dh_x25519_e);
    free(storage_dh_x25519_d);
    free(storage_dh_x25519_c);
    free(storage_dh_x25519_b);
    free(storage_dh_x25519_a);
}
#undef crypto_dh_SECRETKEYBYTES
#undef crypto_dh_PUBLICKEYBYTES
#undef crypto_dh_BYTES
