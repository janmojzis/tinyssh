/*
20140204
Jan Mojzis
Public domain.
*/

#include "crypto.h"
#include "packetparser.h"
#include "stringparser.h"
#include "str.h"
#include "byte.h"
#include "e.h"
#include "log.h"
#include "bug.h"
#include "purge.h"
#include "sshcrypto.h"

struct sshcrypto_key sshcrypto_keys[] = {
#ifdef crypto_sign_ed25519_BYTES
    {   "ssh-ed25519",
        crypto_sign_ed25519,
        crypto_sign_ed25519_open,
        crypto_sign_ed25519_keypair,
        {0},
        crypto_sign_ed25519_PUBLICKEYBYTES,
        crypto_sign_ed25519_SECRETKEYBYTES,
        crypto_sign_ed25519_BYTES,
        "ed25519.pk",
        ".ed25519.sk",
        sshcrypto_TYPENEWCRYPTO,
        0,
        0,
        ed25519_putsignature,
        ed25519_putsignpk,
        ed25519_putsignpkbase64,
        ed25519_parsesignature,
        ed25519_parsesignpk,
    },
#endif
#ifdef crypto_sign_nistp256ecdsa_BYTES
    {   "ecdsa-sha2-nistp256",
        crypto_sign_nistp256ecdsa,
        crypto_sign_nistp256ecdsa_open,
        crypto_sign_nistp256ecdsa_keypair,
        {0},
        crypto_sign_nistp256ecdsa_PUBLICKEYBYTES,
        crypto_sign_nistp256ecdsa_SECRETKEYBYTES,
        crypto_sign_nistp256ecdsa_BYTES,
        "nistp256ecdsa.pk",
        ".nistp256ecdsa.sk",
        sshcrypto_TYPEOLDCRYPTO,
        0,
        0,
        nistp256ecdsa_putsignature,
        nistp256ecdsa_putsignpk,
        nistp256ecdsa_putsignpkbase64,
        nistp256ecdsa_parsesignature,
        nistp256ecdsa_parsesignpk,
    },
#endif
#if 0
    {   "pqkeyTODO",
        crypto_sign_ed25519,
        crypto_sign_ed25519_open,
        crypto_sign_ed25519_keypair,
        {0},
        crypto_sign_ed25519_PUBLICKEYBYTES,
        crypto_sign_ed25519_SECRETKEYBYTES,
        crypto_sign_ed25519_BYTES,
        "pqkeyTODO.pk",
        ".pqkeyTODO.sk",
        sshcrypto_TYPEPQCRYPTO,
        0,
        0,
        ed25519_putsignature,
        ed25519_putsignpk,
        ed25519_putsignpkbase64,
        ed25519_parsesignature,
        ed25519_parsesignpk,
    },
#endif
    { 0, 0, 0, 0, {0}, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};

const char *sshcrypto_key_name = 0;
int (*sshcrypto_sign)(unsigned char *,unsigned long long *,const unsigned char *,unsigned long long,const unsigned char *) = 0;
unsigned char *sshcrypto_sign_publickey = 0;
long long sshcrypto_sign_publickeybytes = 0;
long long sshcrypto_sign_secretkeybytes = 0;
long long sshcrypto_sign_bytes = 0;
const char *sshcrypto_sign_secretkeyfilename = 0;
void (*sshcrypto_buf_putsignature)(struct buf *, const unsigned char *) = 0;
void (*sshcrypto_buf_putsignpk)(struct buf *, const unsigned char *) = 0;


int sshcrypto_key_select(const unsigned char *buf, long long len) {

    long long i, pos = 0;
    unsigned char *x;
    long long xlen;

    if (sshcrypto_key_name) return 1;

    if (buf[len] != 0) bug_proto();
    log_d2("kex: client: key algorithms: ", (char *)buf);

    for (;;) {
        pos = stringparser(buf, len, pos, &x, &xlen);
        if (!pos) break;

        for (i = 0; sshcrypto_keys[i].name; ++i) {
            if (!sshcrypto_keys[i].sign_flagserver) continue;
            if (str_equaln((char *)x, xlen, sshcrypto_keys[i].name)) {
                sshcrypto_key_name = sshcrypto_keys[i].name;
                sshcrypto_sign = sshcrypto_keys[i].sign;
                sshcrypto_sign_publickey = sshcrypto_keys[i].sign_publickey;
                sshcrypto_sign_publickeybytes = sshcrypto_keys[i].sign_publickeybytes;
                sshcrypto_sign_secretkeybytes = sshcrypto_keys[i].sign_secretkeybytes;
                sshcrypto_sign_bytes = sshcrypto_keys[i].sign_bytes;
                sshcrypto_sign_secretkeyfilename = sshcrypto_keys[i].sign_secretkeyfilename;
                sshcrypto_buf_putsignature = sshcrypto_keys[i].buf_putsignature;
                sshcrypto_buf_putsignpk = sshcrypto_keys[i].buf_putsignpk;
                log_d2("kex: key selected: ", sshcrypto_keys[i].name);
                return 1;
            }
        }
    }
    log_d2("kex: key not available ", (char *)buf);
    errno = EPROTO;
    return 0;
}

void sshcrypto_key_put(struct buf *b) {

    crypto_uint32 len = 0;
    long long i, j, start;

    j = 0;
    for (i = 0; sshcrypto_keys[i].name; ++i) {
        if (!sshcrypto_keys[i].sign_flagserver) continue;
        if (j++) ++len;
        len += str_len(sshcrypto_keys[i].name);
    }

    buf_putnum32(b, len);
    start = b->len;

    j = 0;
    for (i = 0; sshcrypto_keys[i].name; ++i) {
        if (!sshcrypto_keys[i].sign_flagserver) continue;
        if (j++) buf_puts(b, ",");
        buf_puts(b, sshcrypto_keys[i].name);
    }
    b->buf[b->len] = 0;
    log_d2("kex: server: key algorithms: ", (char *)b->buf + start);
}
