/*
20240414
Jó Ágila Bitsch
Public domain.

Specification: https://cvsweb.openbsd.org/cgi-bin/cvsweb/src/usr.bin/ssh/PROTOCOL.u2f?annotate=HEAD
*/

#include "crypto.h"
#include "packetparser.h"
#include "buf.h"
#include "byte.h"
#include "str.h"
#include "purge.h"
#include "sshcrypto.h"

#ifdef crypto_sign_ed25519_BYTES
int sk_ed25519_open(unsigned char *m, unsigned long long *mlen, const unsigned char *sm, unsigned long long n, const unsigned char *pk) {
 
    unsigned char buf[crypto_sign_ed25519_BYTES + crypto_hash_sha256_BYTES + 1 + 4 + crypto_hash_sha256_BYTES];
    unsigned long long buflen = sizeof(buf);
    long long i;
    int ret = 1;

    byte_copy(buf, crypto_sign_ed25519_BYTES, sm);

    // application
    crypto_hash_sha256(buf + crypto_sign_ed25519_BYTES, (const unsigned char *)"ssh:", 4);  // TODO extract from pk once it is actually parsed

    // flags + counter
    byte_copy(buf + crypto_sign_ed25519_BYTES + crypto_hash_sha256_BYTES, 1 + 4, sm + crypto_sign_ed25519_BYTES);

    // message
    crypto_hash_sha256(buf + crypto_sign_ed25519_BYTES + crypto_hash_sha256_BYTES + 1 + 4, sm + crypto_sign_ed25519_BYTES + 5, n - crypto_sign_ed25519_BYTES - 5);

    ret = crypto_sign_ed25519_open(m, mlen, buf, buflen, pk);

    *mlen = n - 69;
    for (i=*mlen; i > 0 ; i--) m[i-1] = sm[i+69-1];

    return ret;
}

void sk_ed25519_putsignature(struct buf *b, const unsigned char *x) {

    const char *name = "sk-ssh-ed25519@openssh.com";
    long long len = crypto_sign_ed25519_BYTES;

    buf_putnum32(b, len + str_len(name) + 8 + 1 + 4);
    buf_putstring(b, name);
    buf_putstringlen(b, x, len);
    // put flag and counter
    buf_put(b, x+crypto_sign_ed25519_BYTES, 5);
}

void sk_ed25519_putsignpk(struct buf *b, const unsigned char *x) {

    const char *name = "sk-ssh-ed25519@openssh.com";
    long long len = crypto_sign_ed25519_PUBLICKEYBYTES;

    buf_putnum32(b, len + str_len(name) + 16);
    buf_putstring(b, name);
    buf_putstringlen(b, x, len);
    // TODO deal with applications other than "ssh:"
    buf_putstringlen(b, (const unsigned char *)"ssh:", 4);
}
void sk_ed25519_putsignpkbase64(struct buf *b, const unsigned char *x) {

    unsigned char buf[34 + crypto_sign_ed25519_PUBLICKEYBYTES + 8];

    byte_copy(buf, 34, "\0\0\0\032sk-ssh-ed25519@openssh.com\0\0\0\040");
    byte_copy(buf + 34, crypto_sign_ed25519_PUBLICKEYBYTES, x);
    // TODO deal with applications other than "ssh:"
    byte_copy(buf + 34 + crypto_sign_ed25519_PUBLICKEYBYTES, 8, "\0\0\0\04ssh:");
    buf_putbase64(b, buf, sizeof buf);
    purge(buf, sizeof buf);
}
int sk_ed25519_parsesignpk(unsigned char *buf, const unsigned char *x, long long xlen) {

    long long pos = 0;
    crypto_uint32 len;

    pos = packetparser_uint32(x, xlen, pos, &len);
    pos = packetparser_skip(x, xlen, pos, len);
    if (!str_equaln((char *)x + pos - len, len, "sk-ssh-ed25519@openssh.com")) return 0;

    // string		public key
    pos = packetparser_uint32(x, xlen, pos, &len);
    if (len != crypto_sign_ed25519_PUBLICKEYBYTES) return 0;
    pos = packetparser_copy(x, xlen, pos, buf, len);

    // string		application (user-specified, but typically "ssh:")
    // TODO deal with applications other than "ssh:"
    pos = packetparser_uint32(x, xlen, pos, &len);
    pos = packetparser_skip(x, xlen, pos, len);
    if (!str_equaln((char *)x + pos - len, len, "ssh:")) return 0;

    pos = packetparser_end(x, xlen, pos);
    return 1;
}
int sk_ed25519_parsesignature(unsigned char *buf, const unsigned char *x, long long xlen) {

    long long pos = 0;
    crypto_uint32 len;

    pos = packetparser_uint32(x, xlen, pos, &len);
    pos = packetparser_skip(x, xlen, pos, len);
    if (!str_equaln((char *)x + pos - len, len, "sk-ssh-ed25519@openssh.com")) return 0;

    pos = packetparser_uint32(x, xlen, pos, &len);
    if (len != crypto_sign_ed25519_BYTES) return 0;
    len = len + 5;
    pos = packetparser_skip(x, xlen, pos, len);
    byte_copy(buf, len, x + pos - len);
    pos = packetparser_end(x, xlen, pos);

    if ((buf[crypto_sign_ed25519_BYTES] & 0x8) != 0) return 0;  // we can't handle extensions yet

    return 1;
}
#endif
