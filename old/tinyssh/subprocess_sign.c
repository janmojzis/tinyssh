/*
20140117
Jan Mojzis
Public domain.
*/

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "load.h"
#include "log.h"
#include "open.h"
#include "writeall.h"
#include "purge.h"
#include "global.h"
#include "bug.h"
#include "e.h"
#include "purge.h"
#include "readall.h"
#include "blocking.h"
#include "sshcrypto.h"
#include "subprocess.h"

/*
The 'subprocess_sign' function reads secret-key from 'keydir' and
sings the data 'x' of length 'xlen' and returns it in 'y'.
Caller is expecting 'y' of length 'ylen'. Signing is done 
in a different process, so secret-key is in a separate
memory space than rest of the program.
*/
int subprocess_sign(unsigned char *y, long long ylen, const char *keydir, unsigned char *x, long long xlen) {

    pid_t pid;
    int status, fromchild[2] = {-1, -1};

    if (ylen != sshcrypto_sign_bytes) bug_inval();
    if (xlen != sshcrypto_hash_bytes) bug_inval();
    if (!y || !keydir || !x) bug_inval();

    if (open_pipe(fromchild) == -1) return -1;
    pid = fork();
    if (pid == -1) { close(fromchild[0]); close(fromchild[1]); return -1; }
    if (pid == 0) {
        unsigned char sk[sshcrypto_sign_SECRETKEYMAX];
        unsigned char sm[sshcrypto_sign_MAX + sshcrypto_hash_MAX];
        unsigned long long smlen;

        close(fromchild[0]);

        /* signing starts here */
        if (chdir(keydir) == -1) {
            log_w2("sign: unable to change directory to ", keydir);
            global_die(111);
        }
        if (load(sshcrypto_sign_secretkeyfilename, sk, sshcrypto_sign_secretkeybytes) == -1) {
            log_w4("sign: unable to load secret-key from file ", keydir, "/", sshcrypto_sign_secretkeyfilename);
            purge(sk, sizeof sk);
            global_die(111);
        }
        if (sshcrypto_sign(sm, &smlen, x, sshcrypto_hash_bytes, sk) != 0) { 
            log_w4("sign: unable to sign using secret-key from file ", keydir, "/", sshcrypto_sign_secretkeyfilename);
            purge(sk, sizeof sk);
            global_die(111);
        }
        purge(sk, sizeof sk);
        if (writeall(fromchild[1], sm, sshcrypto_sign_bytes) == -1) {
            log_w1("sign: unable to write signature to parrent process");
            global_die(111);
        }
        close(fromchild[1]);
        /* signing ends here */

        purge(sm, sizeof sm);
        global_die(0);
    }
    close(fromchild[1]);
    blocking_enable(fromchild[0]);
    if (readall(fromchild[0], y, ylen) == -1) { close(fromchild[0]); return -1; }
    close(fromchild[0]);

    while (waitpid(pid, &status, 0) != pid) {};
    if (!WIFEXITED(status)) return -1;
    return WEXITSTATUS(status);
}
