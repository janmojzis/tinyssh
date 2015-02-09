#include "verify.h"

int verify(const unsigned char *x, const unsigned char *y, long long n) {

    unsigned int d = 0;
    long long i;

    for (i = 0; i < n; ++i) d |= x[i] ^ y[i];
    return (1 & ((d - 1) >> 8)) - 1;
}
