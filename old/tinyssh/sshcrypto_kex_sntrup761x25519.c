/*
20210314
Jan Mojzis
Public domain.
*/

#include "buf.h"
#include "crypto.h"
#include "sshcrypto.h"

#if defined(crypto_kem_sntrup761x25519_BYTES) && defined(crypto_hash_sha512_BYTES)
void sntrup761x25519_putkemkey(struct buf *b, const unsigned char *x) {
    buf_putstringlen(b, x, crypto_kem_sntrup761x25519_BYTES);
}
#endif
