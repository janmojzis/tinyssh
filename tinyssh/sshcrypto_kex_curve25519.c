/*
20140203
Jan Mojzis
Public domain.
*/

#include "buf.h"
#include "crypto.h"
#include "randombytes.h"
#include "sshcrypto.h"

#if defined(crypto_scalarmult_curve25519_BYTES) && defined(crypto_hash_sha256_BYTES)
int curve25519_dh(unsigned char *k, unsigned char *pk, unsigned char *sk) {
    long long i;
    unsigned int d = 0;
    int ret;

    ret = crypto_scalarmult_curve25519(k, sk, pk);
    for (i = 0; i < crypto_scalarmult_curve25519_BYTES; ++i) d |= k[i];
    ret |= -(1 & ((d - 1) >> 8));
    return ret;
}

int curve25519_keypair(unsigned char *pk, unsigned char *sk) {

    randombytes(sk, crypto_scalarmult_curve25519_SCALARBYTES);
    return crypto_scalarmult_curve25519_base(pk, sk);
}

void curve25519_putdhpk(struct buf *b, const unsigned char *x) {
    buf_putstringlen(b, x, crypto_scalarmult_curve25519_BYTES);
}

void curve25519_putsharedsecret(struct buf *b, const unsigned char *x) {
    buf_putsharedsecret(b, x, crypto_scalarmult_curve25519_BYTES);
}
#endif
