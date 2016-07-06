/*
20140204
Jan Mojzis
Public domain.
*/

#include "crypto.h"
#include "packetparser.h"
#include "buf.h"
#include "byte.h"
#include "str.h"
#include "purge.h"
#include "sshcrypto.h"

#ifdef crypto_sign_ed25519_BYTES
void ed25519_putsignature(struct buf *b, const unsigned char *x) {

    const char *name = "ssh-ed25519";
    long long len = crypto_sign_ed25519_BYTES;

    buf_putnum32(b, len + str_len(name) + 8);
    buf_putstring(b, name);
    buf_putstringlen(b, x, len);
}

void ed25519_putsignpk(struct buf *b, const unsigned char *x) {

    const char *name = "ssh-ed25519";
    long long len = crypto_sign_ed25519_PUBLICKEYBYTES;

    buf_putnum32(b, len + str_len(name) + 8);
    buf_putstring(b, name);
    buf_putstringlen(b, x, len);
}
void ed25519_putsignpkbase64(struct buf *b, const unsigned char *x) {

    unsigned char buf[19 + crypto_sign_ed25519_PUBLICKEYBYTES];

    byte_copy(buf, 19, "\0\0\0\013ssh-ed25519\0\0\0\040");
    byte_copy(buf + 19, crypto_sign_ed25519_PUBLICKEYBYTES, x);
    buf_putbase64(b, buf, sizeof buf);
    purge(buf, sizeof buf);
}
int ed25519_parsesignpk(unsigned char *buf, const unsigned char *x, long long xlen) {

    long long pos = 0;
    crypto_uint32 len;

    pos = packetparser_uint32(x, xlen, pos, &len);
    pos = packetparser_skip(x, xlen, pos, len);
    if (!str_equaln((char *)x + pos - len, len, "ssh-ed25519")) return 0;

    pos = packetparser_uint32(x, xlen, pos, &len);
    if (len != crypto_sign_ed25519_PUBLICKEYBYTES) return 0;
    pos = packetparser_copy(x, xlen, pos, buf, len);
    pos = packetparser_end(x, xlen, pos);
    return 1;
}
int ed25519_parsesignature(unsigned char *buf, const unsigned char *x, long long xlen) {

    long long pos = 0;
    crypto_uint32 len;

    pos = packetparser_uint32(x, xlen, pos, &len);
    pos = packetparser_skip(x, xlen, pos, len);
    if (!str_equaln((char *)x + pos - len, len, "ssh-ed25519")) return 0;

    pos = packetparser_uint32(x, xlen, pos, &len);
    if (len != crypto_sign_ed25519_BYTES) return 0;
    pos = packetparser_copy(x, xlen, pos, buf, len);
    pos = packetparser_end(x, xlen, pos);
    return 1;
}
#endif
