/*
20241114
Jan Mojzis
Public domain.
*/

#include "buf.h"
#include "crypto.h"
#include "purge.h"
#include "crypto_int16.h"
#include "sshcrypto.h"

#if defined(crypto_dh_x25519_BYTES) && defined(crypto_hash_sha256_BYTES)
int curve25519_enc(unsigned char *c, unsigned char *k, const unsigned char *pk) {

    unsigned char onetimesk[crypto_dh_x25519_SECRETKEYBYTES];
    long long i;
    unsigned int d = 0;

    crypto_dh_x25519_keypair(/*onetimepk*/ c, onetimesk);
    crypto_dh_x25519(k, pk, onetimesk);
    purge(onetimesk, sizeof onetimesk);

    for (i = 0; i < crypto_dh_x25519_BYTES; ++i) d |= k[i];
    return crypto_int16_zero_mask(d);
}

void curve25519_putkemkey(struct buf *b, const unsigned char *x) {
    buf_putsharedsecret(b, x, crypto_dh_x25519_BYTES);
}
#endif
