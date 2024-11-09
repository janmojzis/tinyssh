#include "verify.h"
#include "crypto_verify_32.h"

int crypto_verify_32_tinyssh(const unsigned char *x, const unsigned char *y) {
    return verify(x, y, 32);
}
