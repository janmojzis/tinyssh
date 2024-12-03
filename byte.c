/*
20241205
*/

#include "crypto_int16.h"
#include "byte.h"

void byte_copy(void *yv, long long ylen, const void *xv) {

    long long i;
    const char *x = xv;
    char *y = yv;

    for (i = 0; i < ylen; ++i) y[i] = x[i];
}

int byte_isequal(const void *yv, long long ylen, const void *xv) {

    long long i;
    const unsigned char *y = yv;
    const unsigned char *x = xv;
    unsigned char diff = 0;

    for (i = 0; i < ylen; ++i) diff |= x[i] ^ y[i];
    return crypto_int16_zero_01(diff);
}

void byte_zero(void *yv, long long ylen) {

    long long i;
    volatile char *y = yv;

    for (i = 0; i < ylen; ++i) y[i] = 0;
#ifdef __GNUC__
    __asm__ __volatile__("" : : "r"(yv) : "memory");
#endif
}
