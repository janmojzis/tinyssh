#include "randombytes.h"
#include "crypto_scalarmult_curve25519.h"
#include "crypto_dh_x25519.h"

int crypto_dh_x25519_tinyssh(unsigned char *k, const unsigned char *pk, const unsigned char *sk) {
    return crypto_scalarmult_curve25519(k, sk, pk);
}

static const unsigned char basepoint[32] = {9};

int crypto_dh_x25519_tinyssh_keypair(unsigned char *q, unsigned char *n) {
    randombytes(n, 32);
    return crypto_dh_x25519(q, basepoint, n);
}
