#include "haslib25519.h"
#ifdef HASLIB25519

#include <lib25519.h>
#include "crypto_scalarmult_curve25519.h"
int crypto_scalarmult_curve25519_lib25519(unsigned char *q, const unsigned char *n, const unsigned char *p) {
    long long i;
    unsigned int d = 0;
    lib25519_nP_montgomery25519(q, n, p);
    for (i = 0; i < lib25519_nP_montgomery25519_POINTBYTES; ++i) d |= q[i];
    return -(1 & ((d - 1) >> 8));
}
int crypto_scalarmult_curve25519_lib25519_base(unsigned char *q, const unsigned char *n) {
    long long i;
    unsigned int d = 0;
    lib25519_nG_montgomery25519(q, n);
    for (i = 0; i < lib25519_nG_montgomery25519_POINTBYTES; ++i) d |= q[i];
    return -(1 & ((d - 1) >> 8));
}
#endif
