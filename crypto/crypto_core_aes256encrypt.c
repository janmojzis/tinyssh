#include "aes.h"
#include "cleanup.h"
#include "crypto_core_aes256encrypt.h"

int crypto_core_aes256encrypt_tinynacl(unsigned char *out, const unsigned char *in, const unsigned char *k, const unsigned char *c) {

    unsigned char d[240];
    long long i;

    aes_expandkey256(d, k);
    aes_core(out, in, d, 14);
    cleanup(d);
    return 0;
}
