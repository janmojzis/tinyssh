/*
20141017
Jan Mojzis
Public domain.
*/

#include "misc.h"
#include "crypto_scalarmult_nistp256.h"

#define BYTES crypto_scalarmult_nistp256_BYTES
#define SCALARBYTES crypto_scalarmult_nistp256_SCALARBYTES

static unsigned char pk[BYTES];

static unsigned char S[BYTES] = {
     0xde, 0x24, 0x44, 0xbe, 0xbc, 0x8d, 0x36, 0xe6,
     0x82, 0xed, 0xd2, 0x7e, 0x0f, 0x27, 0x15, 0x08,
     0x61, 0x75, 0x19, 0xb3, 0x22, 0x1a, 0x8f, 0xa0,
     0xb7, 0x7c, 0xab, 0x39, 0x89, 0xda, 0x97, 0xc9,
     0xc0, 0x93, 0xae, 0x7f, 0xf3, 0x6e, 0x53, 0x80,
     0xfc, 0x01, 0xa5, 0xaa, 0xd1, 0xe6, 0x66, 0x59,
     0x70, 0x2d, 0xe8, 0x0f, 0x53, 0xce, 0xc5, 0x76,
     0xb6, 0x35, 0x0b, 0x24, 0x30, 0x42, 0xa2, 0x56

};

static unsigned char R[BYTES] = {
     0x51, 0xd0, 0x8d, 0x5f, 0x2d, 0x42, 0x78, 0x88,
     0x29, 0x46, 0xd8, 0x8d, 0x83, 0xc9, 0x7d, 0x11,
     0xe6, 0x2b, 0xec, 0xc3, 0xcf, 0xc1, 0x8b, 0xed,
     0xac, 0xc8, 0x9b, 0xa3, 0x4e, 0xec, 0xa0, 0x3f,
     0x75, 0xee, 0x68, 0xeb, 0x8b, 0xf6, 0x26, 0xaa,
     0x5b, 0x67, 0x3a, 0xb5, 0x1f, 0x6e, 0x74, 0x4e,
     0x06, 0xf8, 0xfc, 0xf8, 0xa6, 0xc0, 0xcf, 0x30,
     0x35, 0xbe, 0xca, 0x95, 0x6a, 0x7b, 0x41, 0xd5

};

static unsigned char d[SCALARBYTES] = {
     0xc5, 0x1e, 0x47, 0x53, 0xaf, 0xde, 0xc1, 0xe6,
     0xb6, 0xc6, 0xa5, 0xb9, 0x92, 0xf4, 0x3f, 0x8d,
     0xd0, 0xc7, 0xa8, 0x93, 0x30, 0x72, 0x70, 0x8b,
     0x65, 0x22, 0x46, 0x8b, 0x2f, 0xfb, 0x06, 0xfd
};

static void test_vector(void) {

    long long j;
    unsigned char r[BYTES];

    if (crypto_scalarmult_nistp256(r, d, S) != 0) fail("crypto_scalarmult_nistp256() failure");
    for (j = 0; j < BYTES; ++j) if (r[j] != R[j]) fail("crypto_scalarmult_nistp256() failure");
}


static int iszero(const unsigned char *x) {

    unsigned char d = 0;
    long long i;

    for (i = 0; i < SCALARBYTES; ++i) d |= x[i];
    return (256 - (unsigned int)d) >> 8;
}


static unsigned char skdata[1080][32] = {
#include "precomp.data"
};

static unsigned char pkdata[1080][64] = {
#include "precomp_nistp256.data"
};

static void test_base(void) {

    long long i, j;

    checksum_zero();
    for (i = 0; i < 1080; ++i) {
        if (iszero(skdata[i])) {
            if (crypto_scalarmult_nistp256_base(pk, skdata[i]) == 0) {
                fail("crypto_scalarmult_nistp256_base() accepts n = 0");
            }
            continue;
        }
        if (crypto_scalarmult_nistp256_base(pk, skdata[i]) != 0) {
            fail_printdata("sk", skdata[i], SCALARBYTES);
            fail("crypto_scalarmult_nistp256_base() failure, please report it !!!!!!!!!");
        }
        for (j = 0; j < BYTES; ++j) if (pk[j] != pkdata[i][j]) {
            fail_printdata("pk_computed", pk, BYTES);
            fail_printdata("pk_expected", pkdata[i], BYTES);
            fail_printdata("sk", skdata[i], SCALARBYTES);
            fail("crypto_scalarmult_nistp256() failure, please report it !!!!!!!!!");
        }
    }
}

static const unsigned char basepoint[BYTES] = {
    0x6b, 0x17, 0xd1, 0xf2, 0xe1, 0x2c, 0x42, 0x47,
    0xf8, 0xbc, 0xe6, 0xe5, 0x63, 0xa4, 0x40, 0xf2,
    0x77, 0x03, 0x7d, 0x81, 0x2d, 0xeb, 0x33, 0xa0,
    0xf4, 0xa1, 0x39, 0x45, 0xd8, 0x98, 0xc2, 0x96,
    0x4f, 0xe3, 0x42, 0xe2, 0xfe, 0x1a, 0x7f, 0x9b,
    0x8e, 0xe7, 0xeb, 0x4a, 0x7c, 0x0f, 0x9e, 0x16,
    0x2b, 0xce, 0x33, 0x57, 0x6b, 0x31, 0x5e, 0xce,
    0xcb, 0xb6, 0x40, 0x68, 0x37, 0xbf, 0x51, 0xf5
};


static unsigned char test_scalarmult_checksum[32] = {
    0x79, 0x63, 0x0b, 0xe6, 0xf6, 0xcd, 0x4a, 0xb3, 
    0xab, 0xb7, 0x90, 0xf8, 0x86, 0x8a, 0x87, 0x06, 
    0x4e, 0xf6, 0xa0, 0xdd, 0xee, 0x2f, 0xb2, 0x10, 
    0x25, 0x10, 0xef, 0x8b, 0xbb, 0x71, 0xbd, 0x84
};

static void test_scalarmult(void) {

    long long i, j;
    unsigned char outpk[BYTES];

    for (i = 0; i < BYTES; ++i) pk[i] = basepoint[i];

    checksum_zero();
    for (i = 0; i < 1080; ++i) {
        if (iszero(skdata[i])) {
            if (crypto_scalarmult_nistp256(outpk, skdata[i], pk) == 0) {
                fail("crypto_scalarmult_nistp256_base() accepts n = 0");
            }
            continue;
        }

        if (crypto_scalarmult_nistp256(outpk, skdata[i], pk) != 0) {
            fail_printdata("pk", pk, BYTES);
            fail_printdata("sk", skdata[i], SCALARBYTES);
            fail("crypto_scalarmult_nistp256() failure, please report it !!!!!!!!!");
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
        if (crypto_scalarmult_nistp256_base(pk1 + i, sk1 + i) != 0) goto fail;
        if (crypto_scalarmult_nistp256_base(pk2 + i, sk2 + i) != 0) goto fail;
        if (crypto_scalarmult_nistp256(k1 + i, sk1 + i, pk2 + i) != 0) goto fail;
        if (crypto_scalarmult_nistp256(k2 + i, sk2 + i, pk1 + i) != 0) goto fail;
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
