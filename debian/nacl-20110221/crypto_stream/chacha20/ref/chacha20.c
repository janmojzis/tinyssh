#include "crypto_uint32.h"
#include "crypto_stream.h"


static crypto_uint32 load_littleendian(const unsigned char *x) {

    crypto_uint32 y = 0;
    long long i;

    for (i = 3; i >= 0; --i) y = (y << 8) | x[i];
    return y;
}

static void store_littleendian(unsigned char *y, crypto_uint32 x) {

    long long i;

    for (i = 0; i < 4; ++i) { y[i] = x; x >>= 8; }
}

static crypto_uint32 rotate(crypto_uint32 x, int c) { return (x << c) | (x >> (32 - c)); }

#define QUARTERROUND(a,b,c,d) \
  x[a] += x[b]; x[d] = rotate(x[d] ^ x[a], 16); \
  x[c] += x[d]; x[b] = rotate(x[b] ^ x[c], 12); \
  x[a] += x[b]; x[d] = rotate(x[d] ^ x[a],  8); \
  x[c] += x[d]; x[b] = rotate(x[b] ^ x[c],  7);


static void core(unsigned char *out, const unsigned char *n, const unsigned char *k, const unsigned char *c) {

    crypto_uint32 x[16], y[16];
    long long i;

    for (i = 0; i < 4; ++i) {
        x[ 0 + i] = load_littleendian(c + 4 * i);
        x[ 4 + i] = load_littleendian(k + 4 * i);
        x[ 8 + i] = load_littleendian(k + 4 * i + 16);
        x[12 + i] = load_littleendian(n + 4 * i);
    }

    for (i = 0; i < 16; ++i) y[i] = x[i];

    for (i = 20; i > 0; i -= 2) {
        QUARTERROUND( 0, 4, 8, 12)
        QUARTERROUND( 1, 5, 9, 13)
        QUARTERROUND( 2, 6,10, 14)
        QUARTERROUND( 3, 7,11, 15)
        QUARTERROUND( 0, 5,10, 15)
        QUARTERROUND( 1, 6,11, 12)
        QUARTERROUND( 2, 7, 8, 13)
        QUARTERROUND( 3, 4, 9, 14)
    }

    for (i = 0; i < 16; ++i) store_littleendian(out + 4 * i, x[i] + y[i]);
    for (i = 0; i < 16; ++i) x[i] = y[i] = 0;
}

static const unsigned char sigma[16] = "expand 32-byte k";

int crypto_stream_xor(unsigned char *c, const unsigned char *m, unsigned long long l, const unsigned char *nn, const unsigned char *kk) {

    unsigned char n[16], x[64], k[32];
    unsigned int u;
    long long i;

    if (!l) return 0;

    for (i = 0; i < 32; ++i) k[i] = kk[i];
    for (i = 0; i <  8; ++i) n[i] = 0;
    for (i = 0; i <  8; ++i) n[i + 8] = nn[i];

    while (l >= 64) {
        core(x, n, k, sigma);
        for (i = 0; i < 64; ++i) c[i] = m[i] ^ x[i];

        u = 1;
        for (i = 0; i < 8; ++i) {
            u += (unsigned int)n[i];
            n[i] = u;
            u >>= 8;
        }

        l -= 64;
        c += 64;
        m += 64;
    }
    if (l) {
        core(x, n, k, sigma);
        for (i = 0; i < l; ++i) c[i] = m[i] ^ x[i];
    }
    for (i = 0; i < 16; ++i) n[i] = 0;
    for (i = 0; i < 64; ++i) x[i] = 0;
    for (i = 0; i < 32; ++i) k[i] = 0;
    return 0;
}

int crypto_stream(unsigned char *c, unsigned long long l, const unsigned char *n, const unsigned char *k) {

    long long i;

    for (i = 0; i < l; ++i) c[i] = 0;
    return crypto_stream_xor(c, c, l, n, k);
}
