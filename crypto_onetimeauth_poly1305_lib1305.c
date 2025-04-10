#include "haslib1305.h"
#ifdef HASLIB1305

#include <lib1305.h>
#include "crypto_onetimeauth_poly1305.h"

int crypto_onetimeauth_poly1305_lib1305(unsigned char *a,
                                        const unsigned char *m,
                                        unsigned long long n,
                                        const unsigned char *k) {
    poly1305(a, m, (unsigned long long) n, k);
    return 0;
}

int crypto_onetimeauth_poly1305_lib1305_verify(const unsigned char *a,
                                               const unsigned char *m,
                                               unsigned long long n,
                                               const unsigned char *k) {
    return poly1305_verify(a, m, (unsigned long long) n, k);
}

#endif
