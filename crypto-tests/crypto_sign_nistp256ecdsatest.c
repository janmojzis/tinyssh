/*
20141017
Jan Mojzis
Public domain.
*/

#include <string.h>
#include "misc.h"
#include "crypto_sign_nistp256ecdsa.h"

#define MLEN 5232

static unsigned char space[MLEN + 16];
static unsigned char m[MLEN + crypto_sign_nistp256ecdsa_BYTES + 16];
static unsigned char sm[MLEN + crypto_sign_nistp256ecdsa_BYTES + 16];
static unsigned char sk[crypto_sign_nistp256ecdsa_SECRETKEYBYTES + 16];
static unsigned char pk[crypto_sign_nistp256ecdsa_PUBLICKEYBYTES + 16];
static unsigned long long mlen;
static unsigned long long smlen;

static unsigned char test_precomp_checksumsha3[32] = {
    0x5b, 0x66, 0xc6, 0xbd, 0xfc, 0x96, 0xa9, 0x74, 
    0xbb, 0xf3, 0x7d, 0x31, 0xc5, 0xa2, 0xd2, 0x45, 
    0xa6, 0x2b, 0xbf, 0xc7, 0x28, 0x33, 0xd1, 0x76, 
    0xe4, 0xd3, 0x1e, 0x0b, 0x24, 0x26, 0x4b, 0x13
};

static unsigned char test_precomp_checksumsha2[32] = {
    0x85, 0x7b, 0x0d, 0xd2, 0x82, 0xab, 0xaa, 0x86, 
    0x87, 0xfe, 0x2a, 0x83, 0x3b, 0x48, 0xb3, 0x84, 
    0xa2, 0x8f, 0x13, 0xf9, 0x72, 0xfb, 0x86, 0x1e, 
    0x4c, 0x57, 0x65, 0x4c, 0x83, 0xc3, 0x0d, 0x66
};


static unsigned char skdata[1080][32] = {
#include "precomp.data"
};

static unsigned char pkdata[1080][64] = {
#include "precomp_nistp256.data"
};

static int iszero(const unsigned char *x) {

    unsigned char d = 0;
    long long i;

    for (i = 0; i < 32; ++i) d |= x[i];
    return (256 - (unsigned int)d) >> 8;
}

static void test_precomp(void) {

    long long i, j;

    pseudorandombytes(space, sizeof space);

    checksum_zero();
    for (i = 0; i < 1080; ++i) {
        for (j = 31; j >= 0; --j) sk[j     ] = skdata[i][j];
        for (j = 31; j >= 0; --j) sk[j + 32] = skdata[i][j];
        for (j = 63; j >= 0; --j) pk[j     ] = pkdata[i][j];
        if (iszero(sk)) continue;
        crypto_sign_nistp256ecdsa(sm, &smlen, space, MLEN - i, sk);
        checksum(sm, crypto_sign_nistp256ecdsa_BYTES);
        if (crypto_sign_nistp256ecdsa_open(m, &mlen, sm, smlen, pk) != 0) {
            fail_printdata("sm", sm, smlen);
            fail_printdata("pk", pk, crypto_sign_nistp256ecdsa_PUBLICKEYBYTES);
            fail_printdata("sk", sk, crypto_sign_nistp256ecdsa_SECRETKEYBYTES);
            fail("crypto_sign_nistp256ecdsa_open() failure, please report it !!!!!!!!!");
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

        if (crypto_sign_nistp256ecdsa_keypair(pk + i, sk + i) != 0)
            fail("crypto_sign_nistp256ecdsa_keypair() failure");

        crypto_sign_nistp256ecdsa(sm + i, &smlen, space + i, j, sk + i);
        if (crypto_sign_nistp256ecdsa_open(m + i, &mlen, sm + i, smlen, pk + i) != 0) {
            fail_printdata("sm", sm + i, smlen);
            fail_printdata("pk", pk + i, crypto_sign_nistp256ecdsa_PUBLICKEYBYTES);
            fail_printdata("sk", sk + i, crypto_sign_nistp256ecdsa_SECRETKEYBYTES);
            fail("crypto_sign_nistp256ecdsa_open() failure, please report it !!!!!!!!!");
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

    if (crypto_sign_nistp256ecdsa_keypair(pk, sk) != 0) fail("crypto_sign_nistp256ecdsa_keypair() failure");

    pseudorandombytes(m, sizeof m);

    crypto_sign_nistp256ecdsa(sm, &smlen, m + 64, sizeof m - 64, sk);
    if (crypto_sign_nistp256ecdsa_open(om, &omlen, sm, sizeof sm, pk) != 0) {
        fail_printdata("sm", sm, smlen);
        fail_printdata("pk", pk, crypto_sign_nistp256ecdsa_PUBLICKEYBYTES);
        fail_printdata("sk", sk, crypto_sign_nistp256ecdsa_SECRETKEYBYTES);
        fail("crypto_sign_nistp256ecdsa_open() failure, please report it !!!!!!!!!");
    }

    if (memcmp(m + 64, om, sizeof m - 64)) {
        fail("crypto_sign_nistp256ecdsa_open() failure, messages do not match");
    }
}


int main(void) {

    test_precomp();
    test_random();
    test_signopen();

    _exit(0);
}
