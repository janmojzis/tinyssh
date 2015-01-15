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
#include "purge.h"

#define sk global_bspace1 /* reusing global buffer */
#define pk global_bspace2 /* reusing global buffer */

static void die_usage(void) {

    log_u1("tinysshd-makekey keydir");
    global_die(100);
}

static void die_fatal(const char *trouble, const char *d, const char *fn) {

    if (d) {
        if (fn) log_f5(trouble, " ", d, "/", fn);
        else log_f3(trouble, " ", d);
    }
    else {
        log_f1(trouble);
    }
    global_die(111);
}

static void create(const char *d, const char *fn, const unsigned char *x, long long xlen) {
    if (savesync(fn, x, xlen) == -1) die_fatal("unable to create", d, fn);
}


int main(int argc,char **argv) {

    const char *d;
    long long i;

    log_init(1, "tinysshd-makekey", 0);

    if (argc < 2) die_usage();
    if (!argv[0]) die_usage();
    if (!argv[1]) die_usage();
    d = argv[1];

    umask(022);
    if (mkdir(d, 0755) == -1) die_fatal("unable to create directory", d, 0);
    if (chdir(d) == -1) die_fatal("unable to chdir to directory", d, 0);

    for (i = 0; sshcrypto_keys[i].name; ++i) {
        if (sshcrypto_keys[i].sign_keypair(pk, sk) != 0) die_fatal("unable to generate key pair", d, 0);
        umask(022);
        create(d, sshcrypto_keys[i].sign_publickeyfilename, pk, sshcrypto_keys[i].sign_publickeybytes);
        umask(077);
        create(d, sshcrypto_keys[i].sign_secretkeyfilename, sk, sshcrypto_keys[i].sign_secretkeybytes);
    }
    global_die(0); return 111;
}
