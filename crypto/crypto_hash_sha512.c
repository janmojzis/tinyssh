#include "sha512.h"
#include "cleanup.h"
#include "crypto_hash_sha512.h"

int crypto_hash_sha512_tinynacl(unsigned char *o,const unsigned char *m, unsigned long long n) {

    crypto_uint64 h[8] = {
        0x6a09e667f3bcc908ULL, 0xbb67ae8584caa73bULL, 0x3c6ef372fe94f82bULL, 0xa54ff53a5f1d36f1ULL,
        0x510e527fade682d1ULL, 0x9b05688c2b3e6c1fULL, 0x1f83d9abfb41bd6bULL, 0x5be0cd19137e2179ULL
    };

    sha512_hash(o, m, n, h);
    cleanup(h);
    return 0;
}
