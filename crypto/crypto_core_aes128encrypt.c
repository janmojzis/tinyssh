#include "aes.h"
#include "cleanup.h"
#include "crypto_core_aes128encrypt.h"

int crypto_core_aes128encrypt_tinynacl(unsigned char *out, const unsigned char *in, const unsigned char *k, const unsigned char *c) {

    unsigned char d[176];
    long long i;

    aes_expandkey128(d, k);
    aes_core(out, in, d, 10);
    cleanup(d);
    return 0;
}
