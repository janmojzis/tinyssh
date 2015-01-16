#include "crypto.h"
#include "sshcrypto.h"

#if defined(crypto_core_aes256encrypt_KEYBYTES)

int aesctr256_xor(unsigned char *c, const unsigned char *m, unsigned long long l, const unsigned char *nn, const unsigned char *k) {

    unsigned int u;
    unsigned char x[16];
    unsigned char *n = (unsigned char *)nn; /* XXX */
    long long i;

    if (l % 16) return -1;

    while (l >= 16) {
        crypto_core_aes256encrypt(x, n, k, 0);
        for (i = 0; i < 16; ++i) c[i] = m[i] ^ x[i];

        u = 1;
        for (i = 15; i >= 0; --i) {
            u += (unsigned int) n[i];
            n[i] = u;
            u >>= 8;
        }

        l -= 16;
        m += 16;
        c += 16;
    }

    for (i = 0; i < sizeof x; ++i) x[i] = 0;
    return 0;
}

#endif
