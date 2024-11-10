#include "crypto_hash_sha512.h"

#include "haslib25519.h"
#ifdef HASLIB25519
#include <lib25519.h>

int crypto_hash_sha512_lib25519(unsigned char *o,const unsigned char *m, unsigned long long n) {
    lib25519_hash_sha512(o, m, n);
    return 0;
}

#endif
