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
    {   "curve25519-sha256@libssh.org",
        curve25519_dh,
        curve25519_keypair,
        crypto_scalarmult_curve25519_BYTES,       /* pk */
        crypto_scalarmult_curve25519_SCALARBYTES, /* sk */
        crypto_scalarmult_curve25519_BYTES,       /* k  */
        crypto_hash_sha256,
        crypto_hash_sha256_BYTES,
        curve25519_putsharedsecret,
        curve25519_putdhpk,
        sshcrypto_TYPENEWCRYPTO,
        0,
    },
    {   "curve25519-sha256",
        curve25519_dh,
        curve25519_keypair,
        crypto_scalarmult_curve25519_BYTES,       /* pk */
        crypto_scalarmult_curve25519_SCALARBYTES, /* sk */
        crypto_scalarmult_curve25519_BYTES,       /* k  */
        crypto_hash_sha256,
        crypto_hash_sha256_BYTES,
        curve25519_putsharedsecret,
        curve25519_putdhpk,
        sshcrypto_TYPENEWCRYPTO,
        0,
    },
#endif
#if defined(crypto_scalarmult_nistp256_BYTES) && defined(crypto_hash_sha256_BYTES)
    {   "ecdh-sha2-nistp256",
        nistp256_dh,
        nistp256_keypair,
        crypto_scalarmult_nistp256_BYTES + 1,   /* pk */
        crypto_scalarmult_nistp256_SCALARBYTES, /* sk */
        crypto_scalarmult_nistp256_BYTES / 2,   /* k  */
        crypto_hash_sha256,
        crypto_hash_sha256_BYTES,
        nistp256_putsharedsecret,
        nistp256_putdhpk,
        sshcrypto_TYPEOLDCRYPTO,
        0,
    },
#endif
#if 0
    {   "pqkexTODO",
        curve25519_dh,
        curve25519_keypair,
        crypto_scalarmult_curve25519_BYTES,       /* pk */
        crypto_scalarmult_curve25519_SCALARBYTES, /* sk */
        crypto_scalarmult_curve25519_BYTES,       /* k  */
        crypto_hash_sha256,
        crypto_hash_sha256_BYTES,
        curve25519_putsharedsecret,
        curve25519_putdhpk,
        sshcrypto_TYPEPQCRYPTO,
        0,
    },
#endif
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};

const char *sshcrypto_kex_name = 0;
int (*sshcrypto_dh)(unsigned char *, unsigned char *, unsigned char *) = 0;
int (*sshcrypto_dh_keypair)(unsigned char *, unsigned char *) = 0;
long long sshcrypto_dh_publickeybytes = 0;
long long sshcrypto_dh_secretkeybytes = 0;
long long sshcrypto_dh_bytes = 0;
int (*sshcrypto_hash)(unsigned char *, const unsigned char *, unsigned long long) = 0;
long long sshcrypto_hash_bytes = 0;
void (*sshcrypto_buf_putsharedsecret)(struct buf *, const unsigned char *) = 0;
void (*sshcrypto_buf_putdhpk)(struct buf *, const unsigned char *) = 0;


int sshcrypto_kex_select(const unsigned char *buf, long long len, crypto_uint8 *kex_guess) {

    long long i, pos = 0;
    unsigned char *x;
    long long xlen;

    if (sshcrypto_kex_name) return 1;

    if (buf[len] != 0) bug_proto();
    log_d2("kex: client: kex algorithms: ", (char *)buf); 

    *kex_guess = 1;

    for (;;) {
        pos = stringparser(buf, len, pos, &x, &xlen);
        if (!pos) break;

        for (i = 0; sshcrypto_kexs[i].name; ++i) {
            if (!sshcrypto_kexs[i].flagenabled) continue;
            if (str_equaln((char *)x, xlen, sshcrypto_kexs[i].name)) {
                sshcrypto_kex_name = sshcrypto_kexs[i].name;
                sshcrypto_dh = sshcrypto_kexs[i].dh;
                sshcrypto_dh_keypair = sshcrypto_kexs[i].dh_keypair;
                sshcrypto_dh_publickeybytes = sshcrypto_kexs[i].dh_publickeybytes;
                sshcrypto_dh_secretkeybytes = sshcrypto_kexs[i].dh_secretkeybytes;
                sshcrypto_dh_bytes = sshcrypto_kexs[i].dh_bytes;
                sshcrypto_hash = sshcrypto_kexs[i].hash;
                sshcrypto_hash_bytes = sshcrypto_kexs[i].hash_bytes;
                sshcrypto_buf_putsharedsecret = sshcrypto_kexs[i].buf_putsharedsecret;
                sshcrypto_buf_putdhpk = sshcrypto_kexs[i].buf_putdhpk;
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

    buf_putnum32(b, len);
    start = b->len;

    j = 0;
    for (i = 0; sshcrypto_kexs[i].name; ++i) {
        if (!sshcrypto_kexs[i].flagenabled) continue;
        if (j++) buf_puts(b, ",");
        buf_puts(b, sshcrypto_kexs[i].name);
    }
    b->buf[b->len] = 0;
    log_d2("kex: server: kex algorithms: ", (char *)b->buf + start);
}
