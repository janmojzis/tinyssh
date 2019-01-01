/*
20140203
Jan Mojzis
Public domain.
*/

#include "buf.h"
#include "crypto.h"
#include "randombytes.h"
#include "purge.h"
#include "sshcrypto.h"

#if defined(crypto_scalarmult_curve25519_BYTES) && defined(crypto_hash_sha256_BYTES)
int curve25519_enc(unsigned char *c, unsigned char *k, const unsigned char *pk) {

    unsigned char onetimesk[crypto_scalarmult_curve25519_SCALARBYTES];
    int r = 0;
    long long i;
    unsigned int d = 0;

    randombytes(onetimesk, sizeof onetimesk);
    r |= crypto_scalarmult_curve25519_base(/*onetimepk*/ c, onetimesk);
    r |= crypto_scalarmult_curve25519(k, onetimesk, pk);
    for (i = 0; i < crypto_scalarmult_curve25519_BYTES; ++i) d |= k[i];
    r |= -(1 & ((d - 1) >> 8));
    purge(onetimesk, sizeof onetimesk);
    return r;
}

void curve25519_putkemkey(struct buf *b, const unsigned char *x) {
    buf_putsharedsecret(b, x, crypto_scalarmult_curve25519_BYTES);
}
#endif
