/*
20140207
Jan Mojzis
Public domain.
*/

#include "crypto.h"
#include "stringparser.h"
#include "e.h"
#include "log.h"
#include "str.h"
#include "byte.h"
#include "packet.h"
#include "sshcrypto.h"

int default_beforenm(unsigned char *y, const unsigned char *x) {
    byte_copy(y, sshcrypto_cipher_KEYMAX, x);
    return 0;
}

/* from sshcrypto_cipher_chachapoly.c */
extern void chachapoly_packet_put(struct buf *);
extern int chachapoly_packet_get(struct buf *);

/* from sshcrypto_cipher_aesctr.c */
extern void aesctr_packet_put(struct buf *);
extern int aesctr_packet_get(struct buf *);
/* from sshcrypto_cipher_aesctr128.c */
extern int aesctr128_xor(unsigned char *, const unsigned char *, unsigned long long, const unsigned char *, const unsigned char *);
/* from sshcrypto_cipher_aesctr256.c */
extern int aesctr256_xor(unsigned char *, const unsigned char *, unsigned long long, const unsigned char *, const unsigned char *);

struct sshcrypto_cipher sshcrypto_ciphers[] = {
#if defined(crypto_stream_chacha20_KEYBYTES) && defined(crypto_onetimeauth_poly1305_BYTES)
    {   "chacha20-poly1305@openssh.com",
        "chacha20-poly1305@openssh.com",
        crypto_stream_chacha20_xor,
        default_beforenm,
        crypto_onetimeauth_poly1305,
        crypto_stream_chacha20_KEYBYTES * 2,
        8,
        crypto_onetimeauth_poly1305_BYTES,
        chachapoly_packet_put,
        chachapoly_packet_get,
        sshcrypto_TYPENEWCRYPTO | sshcrypto_TYPEPQCRYPTO,
        0
    },
#endif
#if defined(crypto_core_aes128encrypt_KEYBYTES) && defined(crypto_auth_hmacsha256_BYTES)
    {   "aes128-ctr",
        "hmac-sha2-256",
        aesctr128_xor,
        default_beforenm,
        crypto_auth_hmacsha256,
        crypto_core_aes128encrypt_KEYBYTES,
        16,
        crypto_auth_hmacsha256_BYTES,
        aesctr_packet_put,
        aesctr_packet_get,
        sshcrypto_TYPEOLDCRYPTO,
        0
    },
#endif
#if defined(crypto_core_aes256encrypt_KEYBYTES) && defined(crypto_auth_hmacsha256_BYTES)
    {   "aes256-ctr",
        "hmac-sha2-256",
        aesctr256_xor,
        default_beforenm,
        crypto_auth_hmacsha256,
        crypto_core_aes256encrypt_KEYBYTES,
        16,
        crypto_auth_hmacsha256_BYTES,
        aesctr_packet_put,
        aesctr_packet_get,
        sshcrypto_TYPEOLDCRYPTO,
        0
    },
#endif
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};

const char *sshcrypto_cipher_name = 0;
const char *sshcrypto_cipher_macname = 0;
int (*sshcrypto_stream_xor)(unsigned char *, const unsigned char *, unsigned long long, const unsigned char *, const unsigned char *) = 0;
int (*sshcrypto_stream_beforenm)(unsigned char *, const unsigned char *) = 0;
int (*sshcrypto_auth)(unsigned char *, const unsigned char *, unsigned long long, const unsigned char *) = 0;
long long sshcrypto_stream_keybytes = 0;
long long sshcrypto_cipher_blockbytes = 0;
long long sshcrypto_auth_bytes = 0;
void (*sshcrypto_packet_put)(struct buf *) = 0;
int (*sshcrypto_packet_get)(struct buf *) = 0;


int sshcrypto_cipher_select(const unsigned char *buf, long long len) {

    long long i, pos = 0;
    unsigned char *x;
    long long xlen;

    if (sshcrypto_cipher_name) return 1;

    if (buf[len] != 0) { errno = EPROTO; return 0; }
    log_d2("kex: client: cipher algorithms: ", (char *)buf);

    for (;;) {
        pos = stringparser(buf, len, pos, &x, &xlen);
        if (!pos) break;

        for (i = 0; sshcrypto_ciphers[i].name; ++i) {
            if (!sshcrypto_ciphers[i].flagenabled) continue;
            if (str_equaln((char *)x, xlen, sshcrypto_ciphers[i].name)) {
                sshcrypto_cipher_name = sshcrypto_ciphers[i].name;
                sshcrypto_cipher_macname = sshcrypto_ciphers[i].macname;
                sshcrypto_stream_xor = sshcrypto_ciphers[i].stream_xor;
                sshcrypto_stream_beforenm = sshcrypto_ciphers[i].stream_beforenm;
                sshcrypto_auth = sshcrypto_ciphers[i].auth;
                sshcrypto_stream_keybytes = sshcrypto_ciphers[i].stream_keybytes;
                sshcrypto_cipher_blockbytes = sshcrypto_ciphers[i].cipher_blockbytes;
                sshcrypto_auth_bytes = sshcrypto_ciphers[i].auth_bytes;
                sshcrypto_packet_get = sshcrypto_ciphers[i].packet_get;
                sshcrypto_packet_put = sshcrypto_ciphers[i].packet_put;
                log_i2("kex: cipher selected: ", sshcrypto_ciphers[i].name);
                log_i2("kex: mac selected: ", sshcrypto_ciphers[i].macname);
                return 1;
            }
        }
    }
    log_d2("kex: cipher not available", (char *)buf);
    errno = EPROTO;
    return 0;
}


void sshcrypto_cipher_put(struct buf *b) {

    crypto_uint32 len = 0;
    long long i, start;

    for (i = 0; sshcrypto_ciphers[i].name; ++i) {
        if (!sshcrypto_ciphers[i].flagenabled) continue;
        if (i) ++len;
        len += str_len(sshcrypto_ciphers[i].name);
    }

    buf_putnum32(b, len);
    start = b->len;

    for (i = 0; sshcrypto_ciphers[i].name; ++i) {
        if (!sshcrypto_ciphers[i].flagenabled) continue;
        if (i) buf_puts(b, ",");
        buf_puts(b, sshcrypto_ciphers[i].name);
    }
    b->buf[b->len] = 0;
    log_d2("kex: server: cipher algorithms: ", (char *)b->buf + start);
}

void sshcrypto_cipher_macput(struct buf *b) {

    crypto_uint32 len = 0;
    long long i, start;

    for (i = 0; sshcrypto_ciphers[i].macname; ++i) {
        if (!sshcrypto_ciphers[i].flagenabled) continue;
        if (i) ++len;
        len += str_len(sshcrypto_ciphers[i].macname);
    }

    buf_putnum32(b, len);
    start = b->len;

    for (i = 0; sshcrypto_ciphers[i].macname; ++i) {
        if (!sshcrypto_ciphers[i].flagenabled) continue;
        if (i) buf_puts(b, ",");
        buf_puts(b, sshcrypto_ciphers[i].macname);
    }
    b->buf[b->len] = 0;
    log_d2("kex: server: mac algorithms: ", (char *)b->buf + start);
}
