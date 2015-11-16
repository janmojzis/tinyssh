/*
20140417
Jan Mojzis
Public domain.
*/

#include <unistd.h>
#include "sshcrypto.h"

/* grep "define sshcrypto_" source/tinyssh/sshcrypto.h | cut -d ' ' -f2  |  while read x; do echo \#ifndef $x; echo error! ;echo \#endif; done */

#ifndef sshcrypto_dh_PUBLICKEYMAX
error!
#endif
#ifndef sshcrypto_dh_SECRETKEYMAX
error!
#endif
#ifndef sshcrypto_dh_MAX
error!
#endif
#ifndef sshcrypto_hash_MAX
error!
#endif
#ifndef sshcrypto_sign_PUBLICKEYMAX
error!
#endif
#ifndef sshcrypto_sign_SECRETKEYMAX
error!
#endif
#ifndef sshcrypto_sign_MAX
error!
#endif
#ifndef sshcrypto_sign_BASE64PUBLICKEYMAX
error!
#endif
#ifndef sshcrypto_sign_BASE64PUBLICKEYMIN
error!
#endif
#ifndef sshcrypto_cipher_KEYMAX
error!
#endif

int main(void) {

    /* check sshcrypto_init() called called more than once */
    sshcrypto_init();
    sshcrypto_init();
    /* check sshcrypto_purge() called called more than once */
    sshcrypto_purge();
    sshcrypto_purge();
    /* check sshcrypto_init() called called more than once after sshcrypto_purge() */
    sshcrypto_init();
    sshcrypto_init();
    _exit(0);
}
