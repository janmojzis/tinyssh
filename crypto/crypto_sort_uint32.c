#include <stdint.h>
#include "crypto_sort_uint32.h"

static void minmax(uint32_t *x, uint32_t *y) {

    uint32_t xi = *x;
    uint32_t yi = *y;
    uint32_t xy = xi ^ yi;
    uint32_t c = yi - xi;

    c ^= xy & (c ^ yi ^ 0x80000000);
    c >>= 31;
    c &= 1;
    c = -c;
    c &= xy;
    *x = xi ^ c;
    *y = yi ^ c;
}

void crypto_sort_uint32_tinynacl(void *xv, long long n) {

    long long top, p, q, i;
    uint32_t *x = xv;

    if (n < 2) return;
    top = 1;
    while (top < n - top) top += top;

    for (p = top; p > 0; p >>= 1) {
        for (i = 0; i < n - p; ++i)
            if (!(i & p))
                minmax(x + i, x + i + p);
    for (q = top;q > p;q >>= 1)
        for (i = 0; i < n - q; ++i)
            if (!(i & p))
                minmax(x + i + p, x + i + q);
    }
}
