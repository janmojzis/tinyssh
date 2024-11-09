/*
20140203
Jan Mojzis
Public domain.
*/

#include "crypto.h"
#include "stringparser.h"
#include "str.h"
#include "byte.h"
#include "e.h"
#include "log.h"
#include "bug.h"
#include "sshcrypto.h"

struct sshcrypto_kex sshcrypto_kexs[] = {
#if defined(crypto_scalarmult_curve25519_BYTES) && defined(crypto_hash_sha256_BYTES)
    {   "curve25519-sha256",
        curve25519_enc,
        crypto_scalarmult_curve25519_BYTES,     /* pk */
        crypto_scalarmult_curve25519_BYTES,     /* c */
        crypto_scalarmult_curve25519_BYTES,     /* k */
        crypto_hash_sha256,
        crypto_hash_sha256_BYTES,
        curve25519_putkemkey,
        sshcrypto_TYPENEWCRYPTO,
        0,
    },
    {   "curve25519-sha256@libssh.org",
        curve25519_enc,
        crypto_scalarmult_curve25519_BYTES,     /* pk */
        crypto_scalarmult_curve25519_BYTES,     /* c */
        crypto_scalarmult_curve25519_BYTES,     /* k */
        crypto_hash_sha256,
        crypto_hash_sha256_BYTES,
        curve25519_putkemkey,
        sshcrypto_TYPENEWCRYPTO,
        0,
    },
#endif
#if defined(crypto_kem_sntrup761x25519_BYTES) && defined(crypto_hash_sha512_BYTES)
    {   "sntrup761x25519-sha512@openssh.com",
        crypto_kem_sntrup761x25519_enc,
        crypto_kem_sntrup761x25519_PUBLICKEYBYTES,   /* pk */
        crypto_kem_sntrup761x25519_CIPHERTEXTBYTES,  /* c */
        crypto_kem_sntrup761x25519_BYTES,            /* k */
        crypto_hash_sha512,
        crypto_hash_sha512_BYTES,
        sntrup761x25519_putkemkey,
        sshcrypto_TYPEPQCRYPTO,
        0,
    },
#endif
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};

struct sshcrypto_pseudokex sshcrypto_pseudokexs[] = {
    {   "kex-strict-s-v00@openssh.com",
        "kex-strict-c-v00@openssh.com",
        sshcrypto_FLAGSTRICTKEX,
    },
    { 0, 0 }
};

int sshcrypto_kex_flags = 0;
const char *sshcrypto_kex_name = 0;
int (*sshcrypto_enc)(unsigned char *, unsigned char *, const unsigned char *) = 0;
long long sshcrypto_kem_publickeybytes = 0;
long long sshcrypto_kem_ciphertextbytes = 0;
long long sshcrypto_kem_bytes = 0;
int (*sshcrypto_hash)(unsigned char *, const unsigned char *, unsigned long long) = 0;
long long sshcrypto_hash_bytes = 0;
void (*sshcrypto_buf_putkemkey)(struct buf *, const unsigned char *) = 0;


int sshcrypto_kex_select(const unsigned char *buf, long long len, crypto_uint8 *kex_guess) {

    long long i, pos;
    unsigned char *x;
    long long xlen;

    if (sshcrypto_kex_name) return 1;

    if (buf[len] != 0) bug_proto();
    log_d2("kex: client: kex algorithms: ", (char *)buf); 

    *kex_guess = 1;

    pos = 0;
    for (;;) {
        pos = stringparser(buf, len, pos, &x, &xlen);
        if (!pos) break;
        for (i = 0; sshcrypto_pseudokexs[i].name; ++i) {
            if (str_equaln((char *)x, xlen, sshcrypto_pseudokexs[i].cname)) {
                log_d2("kex: pseudokex selected: ", sshcrypto_pseudokexs[i].name);
                sshcrypto_kex_flags |= sshcrypto_pseudokexs[i].flag;
            }
        }
    }

    pos = 0;
    for (;;) {
        pos = stringparser(buf, len, pos, &x, &xlen);
        if (!pos) break;

        for (i = 0; sshcrypto_kexs[i].name; ++i) {
            if (!sshcrypto_kexs[i].flagenabled) continue;
            if (str_equaln((char *)x, xlen, sshcrypto_kexs[i].name)) {
                sshcrypto_kex_name = sshcrypto_kexs[i].name;
                sshcrypto_enc = sshcrypto_kexs[i].enc;
                sshcrypto_kem_publickeybytes = sshcrypto_kexs[i].kem_publickeybytes;
                sshcrypto_kem_ciphertextbytes = sshcrypto_kexs[i].kem_ciphertextbytes;
                sshcrypto_kem_bytes = sshcrypto_kexs[i].kem_bytes;
                sshcrypto_hash = sshcrypto_kexs[i].hash;
                sshcrypto_hash_bytes = sshcrypto_kexs[i].hash_bytes;
                sshcrypto_buf_putkemkey = sshcrypto_kexs[i].buf_putkemkey;
                log_d2("kex: kex selected: ", sshcrypto_kexs[i].name);
                return 1;
            }
        }
        *kex_guess = 0;
    }
    log_d2("kex: kex not available ", (char *)buf);
    errno = EPROTO;
    return 0;
}

void sshcrypto_kex_put(struct buf *b) {

    crypto_uint32 len = 0;
    long long i, j, start;

    j = 0;
    for (i = 0; sshcrypto_kexs[i].name; ++i) {
        if (!sshcrypto_kexs[i].flagenabled) continue;
        if (j++) ++len;
        len += str_len(sshcrypto_kexs[i].name);
    }
    for (i = 0; sshcrypto_pseudokexs[i].name; ++i) {
        if (j++) ++len;
        len += str_len(sshcrypto_pseudokexs[i].name);
    }

    buf_putnum32(b, len);
    start = b->len;

    j = 0;
    for (i = 0; sshcrypto_kexs[i].name; ++i) {
        if (!sshcrypto_kexs[i].flagenabled) continue;
        if (j++) buf_puts(b, ",");
        buf_puts(b, sshcrypto_kexs[i].name);
    }
    for (i = 0; sshcrypto_pseudokexs[i].name; ++i) {
        if (j++) buf_puts(b, ",");
        buf_puts(b, sshcrypto_pseudokexs[i].name);
    }
    b->buf[b->len] = 0;
    log_d2("kex: server: kex algorithms: ", (char *)b->buf + start);
}
