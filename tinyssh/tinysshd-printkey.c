/*
20121022
Jan Mojzis
Public domain.
*/

#include <unistd.h>
#include "log.h"
#include "e.h"
#include "load.h"
#include "sshcrypto.h"
#include "crypto.h"
#include "buf.h"

static unsigned char bspace[256];
static struct buf b = { bspace, 0, sizeof bspace };

static unsigned char pk[sshcrypto_sign_PUBLICKEYMAX];
static unsigned char hashedpk[crypto_hash_md5_BYTES];
static char hexfp[3 * crypto_hash_md5_BYTES];
static char hexpk[2 * sshcrypto_sign_PUBLICKEYMAX + 1];

static void die_usage(void) {

    log_u1("tinysshd-printkey keydir");
    _exit(100);
}

static void die_fatal(const char *trouble, const char *d, const char *fn) {

    if (d) {
        if (fn) log_f5(trouble, " ", d, "/", fn);
        else log_f3(trouble, " ", d);
    }
    else {
        log_f1(trouble);
    }
    _exit(111);
}

int main(int argc, char **argv) {

    char *d;
    long long i, j;

    log_init(3, "tinysshd-printkey", 0);

    if (argc < 2) die_usage();
    if (!argv[0]) die_usage();
    if (!argv[1]) die_usage();
    d = argv[1];

    if (chdir(d) == -1) die_fatal("unable to chdir to directory", d, 0);

    /* read public keys */
    for (i = 0; sshcrypto_keys[i].name; ++i) {
        if (load(sshcrypto_keys[i].sign_publickeyfilename, pk, sshcrypto_keys[i].sign_publickeybytes) == -1) {
            if (errno == ENOENT) continue;
            die_fatal("unable to read public key from file", d, sshcrypto_keys[i].sign_publickeyfilename);
        }

        /* md5 - fingerprint */
        buf_purge(&b);
        sshcrypto_keys[i].buf_putsignpk(&b, pk);
        if (b.len < 4) die_fatal("unable to get publickey", 0, 0);
        crypto_hash_md5(hashedpk, b.buf + 4, b.len - 4);
        for (j = 0; j < crypto_hash_md5_BYTES; ++j) {
            hexfp[3 * j + 0] = "0123456789abcdef"[15 & (int) (hashedpk[j] >> 4)];
            hexfp[3 * j + 1] = "0123456789abcdef"[15 & (int) (hashedpk[j] >> 0)];
            hexfp[3 * j + 2] = ':';
        }
        hexfp[3 * j - 1] = 0;

        /* hex */
        for (j = 0; j < sshcrypto_keys[i].sign_publickeybytes; ++j) {
            hexpk[2 * j + 0] = "0123456789abcdef"[15 & (int) (pk[j] >> 4)];
            hexpk[2 * j + 1] = "0123456789abcdef"[15 & (int) (pk[j] >> 0)];
        }
        hexpk[2 * j] = 0;

        log_i5(sshcrypto_keys[i].name, " ", hexfp, " ", hexpk);
    }
    _exit(0);
}
