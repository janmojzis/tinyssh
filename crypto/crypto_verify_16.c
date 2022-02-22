#include "verify.h"
#include "crypto_verify_16.h"

int crypto_verify_16_tinyssh(const unsigned char *x, const unsigned char *y) {
    return verify(x, y, 16);
}
