/*
20140225
Jan Mojzis
Public domain.
*/

#include "purge.h"
#include "sshcrypto.h"

/* Nothing to do. */
void sshcrypto_init(void) {
}

/*
Remove sentitive data from allocated memory.
*/
void sshcrypto_purge(void) {

    long long i;

    /* kex */
    for (i = 0; sshcrypto_kexs[i].name; ++i) {
        purge(&sshcrypto_kexs[i], sizeof(struct sshcrypto_kex));
    }

    /* key */
    for (i = 0; sshcrypto_keys[i].name; ++i) {
        purge(&sshcrypto_keys[i], sizeof(struct sshcrypto_key));
    }

    /* cipher */
    for (i = 0; sshcrypto_ciphers[i].name; ++i) {
        purge(&sshcrypto_ciphers[i], sizeof(struct sshcrypto_cipher));
    }
}
