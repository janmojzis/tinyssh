/*
20140203
Jan Mojzis
Public domain.
*/

#include "buf.h"
#include "crypto.h"
#include "sshcrypto.h"

#if defined(crypto_scalarmult_curve25519_BYTES) && defined(crypto_hash_sha256_BYTES) && crypto_scalarmult_curve25519_SCALARBYTES == 32
int curve25519_dh(unsigned char *k, unsigned char *pk, unsigned char *sk) {
    return crypto_scalarmult_curve25519(k, sk, pk);
}

int curve25519_keypair(unsigned char *pk, unsigned char *sk) {

    sshcrypto_random32(sk);
    return crypto_scalarmult_curve25519_base(pk, sk);
}

void curve25519_putdhpk(struct buf *b, const unsigned char *x) {
    buf_putstringlen(b, x, crypto_scalarmult_curve25519_BYTES);
}

void curve25519_putsharedsecret(struct buf *b, const unsigned char *x) {
    buf_putsharedsecret(b, x, crypto_scalarmult_curve25519_BYTES);
}
#endif
