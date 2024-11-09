#include "crypto_uint32.h"
#include "crypto_sort_uint32.h"

static void minmax(crypto_uint32 *x, crypto_uint32 *y) {

    crypto_uint32 xi = *x;
    crypto_uint32 yi = *y;
    crypto_uint32 xy = xi ^ yi;
    crypto_uint32 c = yi - xi;

    c ^= xy & (c ^ yi ^ 0x80000000);
    c >>= 31;
    c &= 1;
    c = -c;
    c &= xy;
    *x = xi ^ c;
    *y = yi ^ c;
}

void crypto_sort_uint32_tinyssh(void *xv, long long n) {

    long long top, p, q, i;
    crypto_uint32 *x = xv;

    if (n < 2) return;
    top = 1;
    while (top < n - top) top += top;

    for (p = top; p > 0; p >>= 1) {
        for (i = 0; i < n - p; ++i)
            if (!(i & p))
                minmax(x + i, x + i + p);
        for (q = top; q > p; q >>= 1)
            for (i = 0; i < n - q; ++i)
                if (!(i & p))
                    minmax(x + i + p, x + i + q);
    }
}
