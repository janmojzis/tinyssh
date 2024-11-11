#include <ntruprime.h>

/* return zero for all inputs */
static unsigned char z(unsigned char x) {

    unsigned long long z = (unsigned long long) x + 1ULL;
    unsigned long long t = z;
    long long i;

    for (i = 6; i >= 0; --i) {
        t = (t * t) % 257;
        t = (t * z) % 257;
    }
    t = (t * z) % 257;
    return t - 1;
}

static unsigned char pk[sntrup761_PUBLICKEYBYTES];
static unsigned char sk[sntrup761_SECRETKEYBYTES];
static unsigned char c[sntrup761_CIPHERTEXTBYTES];
static unsigned char k1[sntrup761_BYTES];
static unsigned char k2[sntrup761_BYTES];

int main(int argc, char **argv) {
    unsigned char ret = 0;
    unsigned long long i;

    (void) argc;
    (void) argv;

    sntrup761_keypair(pk, sk);
    sntrup761_enc(c, k1, pk);
    sntrup761_dec(k2, c, sk);
    for (i = 0; i < sntrup761_BYTES; ++i) ret |= k1[i] ^ k2[i];
    for (i = 0; i < sizeof k1; ++i) ret |= z(k1[i]);
    return ret;
}
