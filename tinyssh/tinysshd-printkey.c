/*
20150124
Jan Mojzis
Public domain.
*/

#include <unistd.h>
#include "e.h"
#include "load.h"
#include "sshcrypto.h"
#include "crypto.h"
#include "global.h"
#include "buf.h"
#include "log.h"
#include "writeall.h"

static struct buf b1 = { global_bspace1, 0, sizeof global_bspace1 }; /* reusing global buffer */
static struct buf b2 = { global_bspace2, 0, sizeof global_bspace2 }; /* reusing global buffer */
static unsigned char pk[sshcrypto_sign_PUBLICKEYMAX];

#define USAGE "\
\n\
 name:\n\
   tinysshd-printkey - print SSH public-keys\n\
\n\
 syntax:\n\
   tinysshd-printkey keydir\n\
\n\
 description:\n\
   tinysshd-printkey - prints SSH public-keys in base64 format\n\
\n\
 options:\n\
   keydir: directory containing secret and public SSH keys for signing\n\
\n\
"


static void die_usage(void) {

    log_u1(USAGE);
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

int main(int argc, char **argv) {

    char *x;
    long long i;

    log_init(3, "tinysshd-printkey", 0, 0);

    if (argc < 2) die_usage();
    if (!argv[0]) die_usage();
    if (!argv[1]) die_usage();
    x = argv[1];

    if (chdir(x) == -1) die_fatal("unable to chdir to directory", x, 0);

    /* read public keys */
    for (i = 0; sshcrypto_keys[i].name; ++i) {
        if (load(sshcrypto_keys[i].sign_publickeyfilename, pk, sshcrypto_keys[i].sign_publickeybytes) == -1) {
            if (errno == ENOENT) continue;
            die_fatal("unable to read public key from file", x, sshcrypto_keys[i].sign_publickeyfilename);
        }

        buf_purge(&b1);
        sshcrypto_keys[i].buf_putsignpk(&b1, pk);
        if (b1.len < 4) die_fatal("unable to get publickey", 0, 0);

        /* base64 */
        buf_purge(&b2);
        buf_puts(&b2, sshcrypto_keys[i].name);
        buf_puts(&b2, " ");
        buf_putbase64(&b2, b1.buf + 4, b1.len - 4);
        buf_puts(&b2, "\n");
        if (writeall(1, b2.buf, b2.len) == -1) die_fatal("unable to write output", 0, 0);
    }
    global_die(0); return 111;
}
