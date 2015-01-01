/*
20141017
Jan Mojzis
Public domain.
*/

#include "misc.h"
#include "crypto_scalarmult_curve25519.h"

#define BYTES crypto_scalarmult_curve25519_BYTES
#define SCALARBYTES crypto_scalarmult_curve25519_SCALARBYTES

static unsigned char pk[BYTES];

static unsigned char S[BYTES] = { 9 };

static unsigned char R[BYTES] = {
    0xf9, 0xc3, 0xda, 0xc2, 0x10, 0x4c, 0x80, 0xb2,
    0x52, 0xd0, 0xae, 0xec, 0x37, 0x7a, 0xfd, 0x5d,
    0x1e, 0xf2, 0xc8, 0xc3, 0x48, 0xc2, 0x9e, 0x12,
    0xdd, 0xb2, 0xd0, 0xc8, 0xb1, 0x98, 0xff, 0x7f
};

static unsigned char d[SCALARBYTES] = {
    0x56, 0x2c, 0x1e, 0xb5, 0xfd, 0xb2, 0x81, 0x29,
    0xbd, 0x37, 0x49, 0x58, 0x35, 0xd4, 0xb1, 0x30,
    0x7d, 0xdb, 0x57, 0x38, 0x80, 0x12, 0x17, 0x42,
    0xf7, 0x13, 0xf1, 0x05, 0x67, 0x69, 0xd5, 0xbf
};

static void test_vector(void) {

    long long j;
    unsigned char r[BYTES];

    if (crypto_scalarmult_curve25519(r, d, S) != 0) fail("crypto_scalarmult_curve25519() failure");
    for (j = 0; j < BYTES; ++j) if (r[j] != R[j]) fail("crypto_scalarmult_curve25519() failure");
}

static unsigned char skdata[1080][32] = {
#include "precomp.data"
};

static unsigned char pkdata[1080][32] = {
#include "precomp_curve25519.data"
};

static void test_base(void) {

    long long i, j;

    checksum_zero();
    for (i = 0; i < 1080; ++i) {
        if (crypto_scalarmult_curve25519_base(pk, skdata[i]) != 0) {
            fail_printdata("sk", skdata[i], SCALARBYTES);
            fail("crypto_scalarmult_curve25519_base() failure, please report it !!!!!!!!!");
        }
        for (j = 0; j < BYTES; ++j) if (pk[j] != pkdata[i][j]) {
            fail_printdata("pk_computed", pk, BYTES);
            fail_printdata("pk_expected", pkdata[i], BYTES);
            fail_printdata("sk", skdata[i], SCALARBYTES);
            fail("crypto_scalarmult_curve25519() failure, please report it !!!!!!!!!");
        }
    }
}

static const unsigned char basepoint[BYTES] = { 9 };

static unsigned char test_scalarmult_checksum[32] = {
    0xca, 0xf4, 0xa3, 0xbe, 0x00, 0x9c, 0x6f, 0x01, 
    0xb8, 0x4e, 0xf8, 0x0d, 0x0a, 0x17, 0x16, 0x42, 
    0xfe, 0x01, 0x59, 0x40, 0x74, 0xd4, 0xa6, 0x48, 
    0x07, 0x94, 0x95, 0x94, 0xab, 0xa4, 0x5e, 0x6d
};

static void test_scalarmult(void) {

    long long i, j;
    unsigned char outpk[BYTES];

    for (i = 0; i < BYTES; ++i) pk[i] = basepoint[i];

    checksum_zero();
    for (i = 0; i < 1080; ++i) {
        pk[31] |= 128;
        if (crypto_scalarmult_curve25519(outpk, skdata[i], pk) != 0) {
            fail_printdata("pk", pk, BYTES);
            fail_printdata("sk", skdata[i], SCALARBYTES);
            fail("crypto_scalarmult_curve25519() failure, please report it !!!!!!!!!");
        }
        checksum(outpk, BYTES);
        for (j = 0; j < BYTES; ++j) pk[j] = outpk[j];
    }
    fail_whenbadchecksum(test_scalarmult_checksum);
}

static void test_random(void) {

    long long i, j;
    unsigned char sk1[SCALARBYTES + 16];
    unsigned char pk1[BYTES + 16];
    unsigned char k1[BYTES + 16];
    unsigned char sk2[SCALARBYTES + 16];
    unsigned char pk2[BYTES + 16];
    unsigned char k2[BYTES + 16];


    for (i = 0; i < 16; ++i) {
        unsaferandombytes(sk1 + i, SCALARBYTES);
        unsaferandombytes(sk2 + i, SCALARBYTES);
        if (crypto_scalarmult_curve25519_base(pk1 + i, sk1 + i) != 0) goto fail;
        pk1[31 + i] |= 128;
        if (crypto_scalarmult_curve25519_base(pk2 + i, sk2 + i) != 0) goto fail;
        pk2[31 + i] |= 128;
        if (crypto_scalarmult_curve25519(k1 + i, sk1 + i, pk2 + i) != 0) goto fail;
        if (crypto_scalarmult_curve25519(k2 + i, sk2 + i, pk1 + i) != 0) goto fail;
        for (j = 0; j < BYTES; ++j) if (k1[j + i] != k2[j + i]) goto fail;
    }
    return;

fail:
    fail_printdata("sk1", sk1 + i, SCALARBYTES);
    fail_printdata("sk2", sk2 + i, SCALARBYTES);
    fail("crypto_scalarmult_curve25519() failure, please report it !!!!!!!!!");
}

int main(void) {

    test_vector();
    test_base();
    test_scalarmult();
    test_random();
    _exit(0);
}
