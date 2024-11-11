/*
20141017
Jan Mojzis
Public domain.
*/

#include <string.h>
#include "misc.h"
#include "crypto_sign_ed25519.h"

#define MLEN 5232

static unsigned char space[MLEN + 16];
static unsigned char m[MLEN + crypto_sign_ed25519_BYTES + 16];
static unsigned char sm[MLEN + crypto_sign_ed25519_BYTES + 16];
static unsigned char sk[crypto_sign_ed25519_SECRETKEYBYTES + 16];
static unsigned char pk[crypto_sign_ed25519_PUBLICKEYBYTES + 16];
static unsigned long long mlen;
static unsigned long long smlen;

static unsigned char test_precomp_checksumsha2[32] = {
    0x63, 0xe6, 0x3c, 0xcf, 0x4a, 0x50, 0x12, 0xdc, 
    0xc5, 0xec, 0x43, 0x76, 0x52, 0x6c, 0xf1, 0x5c, 
    0x7b, 0x12, 0xb2, 0xa5, 0xee, 0x77, 0x71, 0x63, 
    0xd0, 0x3a, 0xf1, 0x84, 0xe0, 0xd2, 0x06, 0x13
};

static unsigned char test_precomp_checksumsha3[32] = {
    0x18, 0x03, 0xc1, 0x7e, 0xf4, 0x2c, 0x56, 0xce, 
    0xc0, 0xc4, 0xb2, 0xc3, 0x29, 0xf7, 0xa8, 0x8c, 
    0x1e, 0xfe, 0xd2, 0x9c, 0xf3, 0xdb, 0xe5, 0x61, 
    0x2d, 0x22, 0xf9, 0x0c, 0xd9, 0x0f, 0xf2, 0x7f
};

static unsigned char skdata[1080][32] = {
#include "precomp.data"
};

static unsigned char pkdata[1080][32] = {
#include "precomp_ed25519.data"
};


static void test_precomp(void) {

    long long i, j;

    pseudorandombytes(space, sizeof space);

    checksum_zero();
    for (i = 0; i < 1080; ++i) {
        for (j = 31; j >= 0; --j) sk[j     ] = skdata[i][j];
        for (j = 31; j >= 0; --j) sk[j + 32] = pkdata[i][j];
        for (j = 31; j >= 0; --j) pk[j     ] = pkdata[i][j];
        crypto_sign_ed25519(sm, &smlen, space, MLEN - i, sk);
        checksum(sm, crypto_sign_ed25519_BYTES);
        if (crypto_sign_ed25519_open(m, &mlen, sm, smlen, pk) != 0) {
            fail_printdata("sm", sm, smlen);
            fail_printdata("pk", pk, crypto_sign_ed25519_PUBLICKEYBYTES);
            fail_printdata("sk", sk, crypto_sign_ed25519_SECRETKEYBYTES);
            fail("crypto_sign_ed25519_open() failure, please report it !!!!!!!!!");
        }
    }
    if (!checksum_check((test_precomp_checksumsha2)) && !checksum_check((test_precomp_checksumsha3))) {
        fail_whenbadchecksum(test_precomp_checksumsha3);
    }
}

static void test_random(void) {

    long long i, j;


    i = 0;
    for (j = 0; j < sizeof space; j += 1 + j / 16) {

        unsaferandombytes(space + i, j);

        if (crypto_sign_ed25519_keypair(pk + i, sk + i) != 0)
            fail("crypto_sign_ed25519_keypair() failure");

        crypto_sign_ed25519(sm + i, &smlen, space + i, j, sk + i);
        if (crypto_sign_ed25519_open(m + i, &mlen, sm + i, smlen, pk + i) != 0) {
            fail_printdata("sm", sm + i, smlen);
            fail_printdata("pk", pk + i, crypto_sign_ed25519_PUBLICKEYBYTES);
            fail_printdata("sk", sk + i, crypto_sign_ed25519_SECRETKEYBYTES);
            fail("crypto_sign_ed25519_open() failure, please report it !!!!!!!!!");
        }

        ++i;
        i %= 16;
    }
}

static void test_signopen(void) {

    unsigned char m[1024];
    unsigned char sm[1024];
    unsigned char om[1024];
    unsigned long long smlen = 0;
    unsigned long long omlen;

    if (crypto_sign_ed25519_keypair(pk, sk) != 0) fail("crypto_sign_ed25519_keypair() failure");

    pseudorandombytes(m, sizeof m);

    crypto_sign_ed25519(sm, &smlen, m + 64, sizeof m - 64, sk);
    if (crypto_sign_ed25519_open(om, &omlen, sm, sizeof sm, pk) != 0) {
        fail_printdata("sm", sm, smlen);
        fail_printdata("pk", pk, crypto_sign_ed25519_PUBLICKEYBYTES);
        fail_printdata("sk", sk, crypto_sign_ed25519_SECRETKEYBYTES);
        fail("crypto_sign_ed25519_open() failure, please report it !!!!!!!!!");
    }

    if (memcmp(m + 64, om, sizeof m - 64)) {
        fail("crypto_sign_ed25519_open() failure, messages do not match");
    }
}


int main(void) {

    test_precomp();
    test_random();
    test_signopen();

    _exit(0);
}
