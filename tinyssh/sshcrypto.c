/*
20140225
Jan Mojzis
Public domain.
*/

#include "purge.h"
#include "bug.h"
#include "str.h"
#include "sshcrypto.h"

/*
Initialize and check *MAX constants
*/
void sshcrypto_init(void) {

    long long i;

    /* kex */
    for (i = 0; sshcrypto_kexs[i].name; ++i) {
        if (sshcrypto_kexs[i].dh_publickeybytes > sshcrypto_dh_PUBLICKEYMAX) bug_inval();
        if (sshcrypto_kexs[i].dh_secretkeybytes > sshcrypto_dh_SECRETKEYMAX) bug_inval();
        if (sshcrypto_kexs[i].dh_bytes > sshcrypto_dh_MAX) bug_inval();
        if (sshcrypto_kexs[i].hash_bytes > sshcrypto_hash_MAX) bug_inval();
    }


    /* key */
    for (i = 0; sshcrypto_keys[i].name; ++i) {
        if (sshcrypto_keys[i].sign_publickeybytes > sshcrypto_sign_PUBLICKEYMAX) bug_inval();
        if (sshcrypto_keys[i].sign_secretkeybytes > sshcrypto_sign_SECRETKEYMAX) bug_inval();
        if (sshcrypto_keys[i].sign_bytes > sshcrypto_sign_MAX) bug_inval();
        if (str_len(sshcrypto_keys[i].name) + 1 > sshcrypto_sign_NAMEMAX) bug_inval();
    }


    /* cipher */
    for (i = 0; sshcrypto_ciphers[i].name; ++i) {
        if (sshcrypto_ciphers[i].stream_keybytes > sshcrypto_cipher_KEYMAX) bug_inval();
    }
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
