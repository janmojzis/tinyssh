#include "crypto_int16.h"
#include "crypto_verify_16.h"

int crypto_verify_16_tinyssh(const unsigned char *x, const unsigned char *y) {

    crypto_int16 d = 0;
    long long i;

    for (i = 0; i < crypto_verify_16_BYTES; ++i) { d |= x[i] ^ y[i]; }

    return crypto_int16_nonzero_mask(d);
}
