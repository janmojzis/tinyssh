/*
20210316
Jan Mojzis
Public domain.
*/

#include "misc.h"
#include "crypto_kem_sntrup761x25519.h"
#include "crypto_kem_sntrup761x25519test.data"

unsigned char sk[crypto_kem_sntrup761x25519_SECRETKEYBYTES + 16];
unsigned char pk[crypto_kem_sntrup761x25519_PUBLICKEYBYTES + 16];
unsigned char c[crypto_kem_sntrup761x25519_CIPHERTEXTBYTES + 16];
unsigned char k1[crypto_kem_sntrup761x25519_BYTES + 16];
unsigned char k2[crypto_kem_sntrup761x25519_BYTES + 16];
#define xk2 k2

static void test_random(void) {

    long long i, j;

    for (i = 0; i < 16; ++i) {
        if (crypto_kem_sntrup761x25519_keypair(pk + i, sk + i) != 0) {
            fail_printdata("pk", pk + i, crypto_kem_sntrup761x25519_PUBLICKEYBYTES);
            fail_printdata("sk", sk + i, crypto_kem_sntrup761x25519_SECRETKEYBYTES);
            fail("crypto_kem_sntrup761x25519_keypair() failure, please report it !!!!!!!!!");
        }
        if (crypto_kem_sntrup761x25519_enc(c + i, k1 + i, pk + i) != 0) {
            fail_printdata("pk", pk + i, crypto_kem_sntrup761x25519_PUBLICKEYBYTES);
            fail_printdata("sk", sk + i, crypto_kem_sntrup761x25519_SECRETKEYBYTES);
            fail_printdata("c", c + i, crypto_kem_sntrup761x25519_CIPHERTEXTBYTES);
            fail_printdata("k1", k1 + i, crypto_kem_sntrup761x25519_BYTES);
            fail("crypto_kem_sntrup761x25519_enc() failure, please report it !!!!!!!!!");
        }
        if (crypto_kem_sntrup761x25519_dec(k2 + i, c + i, sk + i) != 0) {
            fail_printdata("pk", pk + i, crypto_kem_sntrup761x25519_PUBLICKEYBYTES);
            fail_printdata("sk", sk + i, crypto_kem_sntrup761x25519_SECRETKEYBYTES);
            fail_printdata("c", c + i, crypto_kem_sntrup761x25519_CIPHERTEXTBYTES);
            fail_printdata("k1", k1 + i, crypto_kem_sntrup761x25519_BYTES);
            fail_printdata("k2", k2 + i, crypto_kem_sntrup761x25519_BYTES);
            fail("crypto_kem_sntrup761x25519_dec() failure, please report it !!!!!!!!!");
        }
        for (j = 0; j < crypto_kem_sntrup761x25519_BYTES; ++j) if (k1[i + j] != k2[i + j]) {
            fail_printdata("pk", pk + i, crypto_kem_sntrup761x25519_PUBLICKEYBYTES);
            fail_printdata("sk", sk + i, crypto_kem_sntrup761x25519_SECRETKEYBYTES);
            fail_printdata("c", c + i, crypto_kem_sntrup761x25519_CIPHERTEXTBYTES);
            fail_printdata("k1", k1 + i, crypto_kem_sntrup761x25519_BYTES);
            fail_printdata("k2", k2 + i, crypto_kem_sntrup761x25519_BYTES);
            fail("crypto_kem_sntrup761x25519_dec() failure, please report it !!!!!!!!!");
        }
    }
}

static void test_dec(void) {

    long long j;

    if (crypto_kem_sntrup761x25519_dec(xk2, xc, xsk) != 0) {
        fail_printdata("xk1", xk1, crypto_kem_sntrup761x25519_BYTES);
        fail_printdata("xk2", xk2, crypto_kem_sntrup761x25519_BYTES);
        fail("crypto_kem_sntrup761x25519_dec() failure, please report it !!!!!!!!!");
    }
    for (j = 0; j < crypto_kem_sntrup761x25519_BYTES; ++j) if (xk1[j] != xk2[j]) {
        fail_printdata("xk1", xk1, crypto_kem_sntrup761x25519_BYTES);
        fail_printdata("xk2", xk2, crypto_kem_sntrup761x25519_BYTES);
        fail("crypto_kem_sntrup761x25519_dec() failure, please report it !!!!!!!!!");
    }
}


int main(void) {

    test_dec();
    test_random();

    _exit(0);
}
