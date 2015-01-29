/*
20140203
Jan Mojzis
Public domain.
*/

#include "buf.h"
#include "crypto.h"
#include "byte.h"
#include "purge.h"
#include "randombytes.h"
#include "sshcrypto.h"

#if defined(crypto_scalarmult_nistp256_BYTES) && defined(crypto_hash_sha256_BYTES)
int nistp256_dh(unsigned char *k, unsigned char *pk, unsigned char *sk) {

    unsigned char kk[crypto_scalarmult_nistp256_BYTES];

    if (pk[0] != 4) return -1;
    if (crypto_scalarmult_nistp256(kk, sk, pk + 1) != 0) return -1;
    byte_copy(k, crypto_scalarmult_nistp256_BYTES / 2, kk);
    purge(kk, sizeof kk);
    return 0;
}

int nistp256_keypair(unsigned char *pk, unsigned char *sk) {

    randombytes(sk, crypto_scalarmult_nistp256_SCALARBYTES);
    pk[0] = 4; /* means uncompressed point */
    return crypto_scalarmult_nistp256_base(pk + 1, sk);
}

void nistp256_putdhpk(struct buf *b, const unsigned char *x) {
    buf_putstringlen(b, x, crypto_scalarmult_nistp256_BYTES + 1);
}

void nistp256_putsharedsecret(struct buf *b, const unsigned char *x) {
    buf_putsharedsecret(b, x, crypto_scalarmult_nistp256_BYTES / 2);
}
#endif
