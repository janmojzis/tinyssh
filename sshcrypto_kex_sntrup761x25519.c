/*
20210314
20241210 - reformated using clang-format
Jan Mojzis
Public domain.
*/

#include "buf.h"
#include "crypto.h"
#include "sshcrypto.h"

void sntrup761x25519_putkemkey(struct buf *b, const unsigned char *x) {
    buf_putstringlen(b, x, crypto_kem_sntrup761x25519_BYTES);
}
