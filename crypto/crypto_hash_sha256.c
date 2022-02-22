#include "crypto_uint32.h"
#include "crypto_hash_sha256.h"

static void store64_bigendian(unsigned char *y, unsigned long long x) {

    long long i;

    for (i = 7; i >= 0; --i) { y[i] = x; x >>= 8; }
}

static void store32_bigendian(unsigned char *y, crypto_uint32 x) {

    long long i;

    for (i = 3; i >= 0; --i) { y[i] = x; x >>= 8; }
}


static crypto_uint32 load32_bigendian(const unsigned char *x) {

    crypto_uint32 y = 0;
    long long i;

    for (i = 0; i < 4; ++i) y = (y << 8) | x[i];
    return y;
}


static crypto_uint32 R(crypto_uint32 x, int c) { return (x >> c) | ((x & 0xffffffff) << (32 - c)); }
static crypto_uint32 Ch(crypto_uint32 x,crypto_uint32 y,crypto_uint32 z) { return (x & y) ^ (~x & z); }
static crypto_uint32 Maj(crypto_uint32 x,crypto_uint32 y,crypto_uint32 z) { return (x & y) ^ (x & z) ^ (y & z); }
static crypto_uint32 Sigma0(crypto_uint32 x) { return R(x, 2) ^ R(x,13) ^ R(x,22); }
static crypto_uint32 Sigma1(crypto_uint32 x) { return R(x, 6) ^ R(x,11) ^ R(x,25); }
static crypto_uint32 sigma0(crypto_uint32 x) { return R(x, 7) ^ R(x,18) ^ (x >> 3); }
static crypto_uint32 sigma1(crypto_uint32 x) { return R(x,17) ^ R(x,19) ^ (x >> 10); }

static const crypto_uint32 K[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
    0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
    0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
    0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
    0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
    0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
    0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
    0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
    0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};


static void blocks(crypto_uint32 *z, const unsigned char *m, unsigned long long n) {

    crypto_uint32 b[8], a[8], w[64], t;
    long long i, j;

    for (i = 0; i < 8; ++i) a[i] = z[i];

    while (n >= 64) {
        for (i =  0; i < 16; ++i) w[i] = load32_bigendian(m + 4 * i);
        for (i = 16; i < 64; ++i) w[i] = (sigma1(w[i - 2]) + w[i - 7] + sigma0(w[i - 15]) + w[i - 16]) & 0xffffffff;

        for (i = 0; i < 64; ++i) {
            for (j = 0; j < 8; ++j) b[j] = a[j];
            t = a[7] + Sigma1(a[4]) + Ch(a[4], a[5], a[6]) + K[i] + w[i];
            b[7] = t + Sigma0(a[0]) + Maj(a[0], a[1], a[2]);
            b[3] += t;
            for (j = 0; j < 8; ++j) a[(j + 1) % 8] = b[j] & 0xffffffff;
        }

        for (i = 0; i < 8; ++i) { a[i] += z[i]; a[i] &= 0xffffffff; z[i] = a[i]; }

        m += 64;
        n -= 64;
    }
}



int crypto_hash_sha256_tinyssh(unsigned char *o, const unsigned char *m, unsigned long long n) {

    long long i;
    unsigned char x[128];
    unsigned long long b = n;
    crypto_uint32 h[8] = {
        0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
        0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
    };

    blocks(h, m, n);
    m += n;
    n &= 63;
    m -= n;

    for (i = 0; i < sizeof x; ++i) x[i] = 0;
    for (i = 0; i < n; ++i) x[i] = m[i];
    x[n] = 128;

    n = 128 - 64 * (n < 56);

    store64_bigendian(x + n - 8, b << 3);
    blocks(h, x, n);

    for (i = 0; i < 8; ++i) store32_bigendian(o + 4 * i, h[i]);
    return 0;
}
