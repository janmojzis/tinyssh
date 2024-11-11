#include "haslib25519.h"
#ifdef HASLIB25519

#include <lib25519.h>
#include "crypto_sign_ed25519.h"

int crypto_sign_ed25519_lib25519(unsigned char *sm, unsigned long long *smlen, const unsigned char *m, unsigned long long n, const unsigned char *skorig) {
    long long llsmlen;
    lib25519_sign_ed25519(sm, &llsmlen, m, n, skorig);
    *smlen = (unsigned long long) llsmlen;
    return 0;
}

int crypto_sign_ed25519_lib25519_open(unsigned char *m, unsigned long long *mlen, const unsigned char *sm, unsigned long long n, const unsigned char *pk) {
    long long llmlen;
    int ret;
    ret = lib25519_sign_ed25519_open(m, &llmlen, sm, n, pk);
    *mlen = (unsigned long long) llmlen;
    return ret;
}

int crypto_sign_ed25519_lib25519_keypair(unsigned char *pk, unsigned char *sk) {
    lib25519_sign_ed25519_keypair(pk, sk);
    return 0;
}

#endif
