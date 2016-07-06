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

#ifdef crypto_sign_nistp256ecdsa_BYTES
static void _putmpint(struct buf *b, const unsigned char *x, long long len) {

    long long pos;

    for (pos = 0; pos < len; ++pos) if (x[pos]) break;

    if (x[pos] & 0x80) {
        buf_putnum32(b, len - pos + 1);
        buf_putnum8(b, 0);
    }
    else {
        buf_putnum32(b, len - pos);
    }
    buf_put(b, x + pos, len - pos);
}

void nistp256ecdsa_putsignature(struct buf *b, const unsigned char *x) {

    unsigned char mpintrspace[crypto_sign_nistp256ecdsa_BYTES / 2 + 4 + 1 + 1];
    unsigned char mpintsspace[crypto_sign_nistp256ecdsa_BYTES / 2 + 4 + 1 + 1];
    unsigned char mpintbspace[crypto_sign_nistp256ecdsa_BYTES + 10 + 1];
    unsigned char sigblobspace[4 + 23 + crypto_sign_nistp256ecdsa_BYTES + 10 + 1];
    struct buf mpintr;
    struct buf mpints;
    struct buf mpintb;
    struct buf sigblob;
    long long halflen = crypto_sign_nistp256ecdsa_BYTES / 2;
    const char *name = "ecdsa-sha2-nistp256";

    buf_init(&mpintr, mpintrspace, sizeof mpintrspace);
    buf_init(&mpints, mpintsspace, sizeof mpintsspace);
    buf_init(&mpintb, mpintbspace, sizeof mpintbspace);
    buf_init(&sigblob, sigblobspace, sizeof sigblobspace);

    _putmpint(&mpintr, x,           halflen);
    _putmpint(&mpints, x + halflen, halflen);
    buf_put(&mpintb, mpintr.buf, mpintr.len);
    buf_put(&mpintb, mpints.buf, mpints.len);
    buf_putstring(&sigblob, name);
    buf_putstringlen(&sigblob, mpintb.buf, mpintb.len);
    buf_putstringlen(b, sigblob.buf, sigblob.len);

    purge(mpintrspace, sizeof mpintrspace);
    purge(mpintsspace, sizeof mpintsspace);
    purge(mpintbspace, sizeof mpintbspace);
    purge(sigblobspace, sizeof sigblobspace);
}

void nistp256ecdsa_putsignpk(struct buf *b, const unsigned char *x) {

    const char *name = "ecdsa-sha2-nistp256";
    const char *ecname = "nistp256";
    long long len = crypto_sign_nistp256ecdsa_PUBLICKEYBYTES;

    buf_putnum32(b, len + str_len(name) + str_len(ecname) + 12 + 1);
    buf_putstring(b, name);
    buf_putstring(b, ecname);
    buf_putnum32(b, len + 1);
    buf_putnum8(b, 4);
    buf_put(b, x, crypto_sign_nistp256ecdsa_PUBLICKEYBYTES);
}
void nistp256ecdsa_putsignpkbase64(struct buf *b, const unsigned char *x) {

    unsigned char buf[40 + crypto_sign_nistp256ecdsa_PUBLICKEYBYTES];

    byte_copy(buf, 40, "\0\0\0\023ecdsa-sha2-nistp256\0\0\0\010nistp256\0\0\0\101\004");
    byte_copy(buf + 40, crypto_sign_nistp256ecdsa_PUBLICKEYBYTES, x);
    buf_putbase64(b, buf, sizeof buf);
    purge(buf, sizeof buf);
}
int nistp256ecdsa_parsesignpk(unsigned char *buf, const unsigned char *x, long long xlen) {

    long long pos = 0;
    crypto_uint32 len;
    crypto_uint8 ch;

    pos = packetparser_uint32(x, xlen, pos, &len);
    pos = packetparser_skip(x, xlen, pos, len);
    if (!str_equaln((char *)x + pos - len, len, "ecdsa-sha2-nistp256")) return 0;

    pos = packetparser_uint32(x, xlen, pos, &len);
    pos = packetparser_skip(x, xlen, pos, len);
    if (!str_equaln((char *)x + pos - len, len, "nistp256")) return 0;

    pos = packetparser_uint32(x, xlen, pos, &len);
    if (len != crypto_sign_nistp256ecdsa_PUBLICKEYBYTES + 1) return 0;
    pos = packetparser_uint8(x, xlen, pos, &ch);
    if (ch != 4) return 0;
    pos = packetparser_copy(x, xlen, pos, buf, len - 1);
    pos = packetparser_end(x, xlen, pos);
    return 1;
}
static int _parsempintblob(unsigned char *buf, const unsigned char *x, long long xlen) {

    long long pos = 0, bpos = 0;
    crypto_uint32 len;
    unsigned char bufr[crypto_sign_nistp256ecdsa_BYTES / 2 + 1];
    unsigned char bufs[crypto_sign_nistp256ecdsa_BYTES / 2 + 1];

    byte_zero(bufr, sizeof bufr);
    byte_zero(bufs, sizeof bufs);
    pos = packetparser_uint32(x, xlen, pos, &len);
    bpos = crypto_sign_nistp256ecdsa_BYTES / 2 - len + 1;
    if (bpos < 0) return 0;
    pos = packetparser_copy(x, xlen, pos, bufr + bpos, len);

    pos = packetparser_uint32(x, xlen, pos, &len);
    bpos = crypto_sign_nistp256ecdsa_BYTES / 2 - len + 1;
    if (bpos < 0) return 0;
    pos = packetparser_copy(x, xlen, pos, bufs + bpos, len);

    byte_copy(buf, crypto_sign_nistp256ecdsa_BYTES / 2, bufr + 1);
    byte_copy(buf + crypto_sign_nistp256ecdsa_BYTES / 2, crypto_sign_nistp256ecdsa_BYTES / 2, bufs + 1);
    purge(bufr, sizeof bufr);
    purge(bufs, sizeof bufs);
    pos = packetparser_end(x, xlen, pos);
    return 1;
}

int nistp256ecdsa_parsesignature(unsigned char *buf, const unsigned char *x, long long xlen) {

    long long pos = 0;
    crypto_uint32 len;

    pos = packetparser_uint32(x, xlen, pos, &len);
    pos = packetparser_skip(x, xlen, pos, len);
    if (!str_equaln((char *)x + pos - len, len, "ecdsa-sha2-nistp256")) return 0;

    pos = packetparser_uint32(x, xlen, pos, &len);
    pos = packetparser_skip(x, xlen, pos, len);
    if (!_parsempintblob(buf, x + pos - len, len)) return 0;
    pos = packetparser_end(x, xlen, pos);
    return 1;
}
#endif
