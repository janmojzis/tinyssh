/*
20140321
Jan Mojzis
Public domain.
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "fail.h"
#include "savesync.h"
#include "sshcrypto.h"
#include "open.h"
#include "purge.h"
#include "byte.h"
#include "randommod.h"
#include "crypto_uint32.h"
#include "subprocess.h"

unsigned char sh[sshcrypto_hash_MAX];
unsigned char sm[sshcrypto_sign_MAX + sshcrypto_hash_MAX];
unsigned char m[sshcrypto_sign_MAX + sshcrypto_hash_MAX];
unsigned long long mlen;
unsigned char sk[sshcrypto_sign_SECRETKEYMAX];
const char *keydir = "./keydir";

static void create(const char *fn, const unsigned char *x, long long xlen) {
    if (savesync(fn, x, xlen) == -1) fail("unable to create test directory");
}

int main(void) {

    long long i, j;
    int fd;

    fd = open_cwd();
    if (fd == -1) fail("open_cwd() failure");

    /* make keydir */
    umask(022);
    if (mkdir(keydir, 0755) == -1) fail("unable to create test directory");
    if (chdir(keydir) == -1) fail("unable to chdir to directory");
    for (i = 0; sshcrypto_keys[i].name; ++i) {
        
        if (sshcrypto_keys[i].sign_flagserver) continue;
        if (sshcrypto_keys[i].sign_keypair(sshcrypto_keys[i].sign_publickey, sk) != 0) fail("unable to generate key pair");
        umask(022);
        create(sshcrypto_keys[i].sign_publickeyfilename, sshcrypto_keys[i].sign_publickey, sshcrypto_keys[i].sign_publickeybytes);
        umask(077);
        create(sshcrypto_keys[i].sign_secretkeyfilename, sk, sshcrypto_keys[i].sign_secretkeybytes);
        purge(sk, sizeof sk);
    }

    if (fchdir(fd) == -1) fail("fchdir() failure");

    for (i = 0; sshcrypto_keys[i].name; ++i) {

        if (sshcrypto_keys[i].sign_flagserver) continue;

        /* set globals */
        sshcrypto_key_name = sshcrypto_keys[i].name;
        sshcrypto_sign = sshcrypto_keys[i].sign;
        sshcrypto_sign_publickey = sshcrypto_keys[i].sign_publickey;
        sshcrypto_sign_publickeybytes = sshcrypto_keys[i].sign_publickeybytes;
        sshcrypto_sign_secretkeybytes = sshcrypto_keys[i].sign_secretkeybytes;
        sshcrypto_sign_bytes = sshcrypto_keys[i].sign_bytes;
        sshcrypto_sign_secretkeyfilename = sshcrypto_keys[i].sign_secretkeyfilename;
        sshcrypto_hash_bytes = 64;

        for (j = 0; j < 100; ++j) {
            /* sign */
            purge(sh, sizeof sh);
            if (subprocess_sign(sm, sshcrypto_sign_bytes, keydir, sh, sshcrypto_hash_bytes) != 0)
                fail("unable to sign");
            byte_copy(sm + sshcrypto_sign_bytes, sshcrypto_hash_bytes, sh);

            /* verify */
            if (sshcrypto_keys[i].sign_open(m, &mlen, sm, sshcrypto_hash_bytes + sshcrypto_sign_bytes, sshcrypto_keys[i].sign_publickey) != 0) 
                fail("unable to open signed box");

#if 0
            /* forgery */
            sm[randommod(sshcrypto_sign_bytes + sshcrypto_hash_bytes)] += 1 + randommod(255);
            if (sshcrypto_keys[i].sign_open(m, &mlen, sm, sshcrypto_hash_bytes + sshcrypto_sign_bytes, sshcrypto_keys[i].sign_publickey) == 0) 
                fail("forgery");
#endif
        }
    }

    _exit(0);
}
