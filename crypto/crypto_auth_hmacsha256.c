#include "sha256.h"
#include "crypto_verify_32.h"
#include "crypto_auth_hmacsha256.h"

int crypto_auth_hmacsha256_tinynacl(unsigned char *out, const unsigned char *m, unsigned long long n, const unsigned char *k) {
    return sha256_auth(out, m, n, k);
}

int crypto_auth_hmacsha256_tinynacl_verify(const unsigned char *h, const unsigned char *m, unsigned long long n, const unsigned char *k) {

    unsigned char x[32];
    crypto_auth_hmacsha256_tinynacl(x, m, n, k);
    return crypto_verify_32(h, x);
}
