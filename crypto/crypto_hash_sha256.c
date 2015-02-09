#include "sha256.h"
#include "crypto_hash_sha256.h"

int crypto_hash_sha256_tinynacl(unsigned char *out, const unsigned char *m, unsigned long long n) {
    return sha256_hash(out, m, n);
}
