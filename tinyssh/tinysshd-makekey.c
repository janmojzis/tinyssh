/*
20121022
Jan Mojzis
Public domain.
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "savesync.h"
#include "log.h"
#include "sshcrypto.h"
#include "global.h"
#include "die.h"

#define sk global_bspace1 /* reusing global buffer */
#define pk global_bspace2 /* reusing global buffer */

#define USAGE "\
\n\
 name:\n\
   tinysshd-makekey - create SSH keys\n\
\n\
 syntax:\n\
   tinysshd-makekey keydir\n\
\n\
 description:\n\
   tinysshd-makekey - creates SSH keys for signing\n\
\n\
 options:\n\
   keydir: directory for secret and public SSH keys for signing\n\
\n\
"

static void create(const char *d, const char *fn, const unsigned char *x, long long xlen) {
    if (savesync(fn, x, xlen) == -1) die_fatal("unable to create", d, fn);
}

int main(int argc, char **argv) {

    char *x;
    long long i;

    log_init(1, "tinysshd-makekey", 0, 0);

    if (argc < 2) die_usage(USAGE);
    if (!argv[0]) die_usage(USAGE);
    for (;;) {
        if (!argv[1]) break;
        if (argv[1][0] != '-') break;
        x = *++argv;
        if (x[0] == '-' && x[1] == 0) break;
        if (x[0] == '-' && x[1] == '-' && x[2] == 0) break;
        while (*++x) {
            die_usage(USAGE);
        }
    }
    x = *++argv; if (!x) die_usage(USAGE);

    umask(022);
    if (mkdir(x, 0755) == -1) die_fatal("unable to create directory", x, 0);
    if (chdir(x) == -1) die_fatal("unable to chdir to directory", x, 0);

    for (i = 0; sshcrypto_keys[i].name; ++i) {
        if (sshcrypto_keys[i].sign_keypair(pk, sk) != 0) die_fatal("unable to generate key pair", x, 0);
        umask(022);
        create(x, sshcrypto_keys[i].sign_publickeyfilename, pk, sshcrypto_keys[i].sign_publickeybytes);
        umask(077);
        create(x, sshcrypto_keys[i].sign_secretkeyfilename, sk, sshcrypto_keys[i].sign_secretkeybytes);
    }
    global_die(0); return 111;
}
