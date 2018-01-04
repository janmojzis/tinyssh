/*
20170509
Jan Mojzis
Public domain.
*/

#include "crypto_uint32.h"
#include "crypto_uint64.h"
#include "crypto_stream_chacha20.h"

static crypto_uint32 unpack32(const unsigned char *x) {
    return
        (crypto_uint32) (x[0])                  \
    | (((crypto_uint32) (x[1])) << 8)           \
    | (((crypto_uint32) (x[2])) << 16)          \
    | (((crypto_uint32) (x[3])) << 24);
}
static void pack32(unsigned char *x, crypto_uint32 u) {
    x[0] = u; u >>= 8;
    x[1] = u; u >>= 8;
    x[2] = u; u >>= 8;
    x[3] = u;
}

#define ROTATE32(x, c) ((x) << (c)) | (((x) & 0xffffffff) >> (32 - (c)))

#define QUARTERROUND(a, b, c, d)                    \
    a += b; d = ROTATE32(d ^ a, 16);                \
    c += d; b = ROTATE32(b ^ c, 12);                \
    a += b; d = ROTATE32(d ^ a,  8);                \
    c += d; b = ROTATE32(b ^ c,  7);

#define TWOROUNDS                                   \
    QUARTERROUND( x0, x4,  x8, x12)                 \
    QUARTERROUND( x1, x5,  x9, x13)                 \
    QUARTERROUND( x2, x6, x10, x14)                 \
    QUARTERROUND( x3, x7, x11, x15)                 \
    QUARTERROUND( x0, x5, x10, x15)                 \
    QUARTERROUND( x1, x6, x11, x12)                 \
    QUARTERROUND( x2, x7,  x8, x13)                 \
    QUARTERROUND( x3, x4,  x9, x14)

#define XORBLOCK(o, i)                              \
    x0  = s0;                                       \
    x1  = s1;                                       \
    x2  = s2;                                       \
    x3  = s3;                                       \
    x4  = k0;                                       \
    x5  = k1;                                       \
    x6  = k2;                                       \
    x7  = k3;                                       \
    x8  = k4;                                       \
    x9  = k5;                                       \
    x10 = k6;                                       \
    x11 = k7;                                       \
    x12 = n0;                                       \
    x13 = n1;                                       \
    x14 = n2;                                       \
    x15 = n3;                                       \
                                                    \
    TWOROUNDS /* round  1,  2 */                    \
    TWOROUNDS /* round  3,  4 */                    \
    TWOROUNDS /* round  5,  6 */                    \
    TWOROUNDS /* round  7,  8 */                    \
    TWOROUNDS /* round  9, 10 */                    \
    TWOROUNDS /* round 11, 12 */                    \
    TWOROUNDS /* round 13, 14 */                    \
    TWOROUNDS /* round 15, 16 */                    \
    TWOROUNDS /* round 17, 18 */                    \
    TWOROUNDS /* round 19, 20 */                    \
                                                    \
    pack32(o     ,  (x0 + s0) ^ unpack32(i     ));  \
    pack32(o +  4,  (x1 + s1) ^ unpack32(i +  4));  \
    pack32(o +  8,  (x2 + s2) ^ unpack32(i +  8));  \
    pack32(o + 12,  (x3 + s3) ^ unpack32(i + 12));  \
    pack32(o + 16,  (x4 + k0) ^ unpack32(i + 16));  \
    pack32(o + 20,  (x5 + k1) ^ unpack32(i + 20));  \
    pack32(o + 24,  (x6 + k2) ^ unpack32(i + 24));  \
    pack32(o + 28,  (x7 + k3) ^ unpack32(i + 28));  \
    pack32(o + 32,  (x8 + k4) ^ unpack32(i + 32));  \
    pack32(o + 36,  (x9 + k5) ^ unpack32(i + 36));  \
    pack32(o + 40, (x10 + k6) ^ unpack32(i + 40));  \
    pack32(o + 44, (x11 + k7) ^ unpack32(i + 44));  \
    pack32(o + 48, (x12 + n0) ^ unpack32(i + 48));  \
    pack32(o + 52, (x13 + n1) ^ unpack32(i + 52));  \
    pack32(o + 56, (x14 + n2) ^ unpack32(i + 56));  \
    pack32(o + 60, (x15 + n3) ^ unpack32(i + 60));


int crypto_stream_chacha20_tinynacl_xor(unsigned char *c, const unsigned char *m, unsigned long long l, const unsigned char *n, const unsigned char *k) {

    crypto_uint32 x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15;
    crypto_uint32 k0 = unpack32(k     );
    crypto_uint32 k1 = unpack32(k +  4);
    crypto_uint32 k2 = unpack32(k +  8);
    crypto_uint32 k3 = unpack32(k + 12);
    crypto_uint32 k4 = unpack32(k + 16);
    crypto_uint32 k5 = unpack32(k + 20);
    crypto_uint32 k6 = unpack32(k + 24);
    crypto_uint32 k7 = unpack32(k + 28);
    crypto_uint32 n0 = 0;
    crypto_uint32 n1 = 0;
    crypto_uint32 n2 = unpack32(n    );
    crypto_uint32 n3 = unpack32(n + 4);
    crypto_uint32 s0 = 0x61707865;
    crypto_uint32 s1 = 0x3320646E;
    crypto_uint32 s2 = 0x79622D32;
    crypto_uint32 s3 = 0x6B206574;
    crypto_uint64 u = 0;

    if (!l) return 0;

    while (l >= 64) {
        XORBLOCK(c, m);

        ++u;
        if (!u) return -1;
        n0 = (crypto_uint32)(u & 0xffffffff);
        n1 = (crypto_uint32)(u >> 32);

        l -= 64;
        c += 64;
        m += 64;
    }
    if (l) {
        unsigned char bspace[64] = {0};
        unsigned char *b = bspace;
        long long j;

        for (j = 0; j < l; ++j) b[j] = m[j];
        XORBLOCK(b, b);
        for (j = 0; j < l; ++j) c[j] = b[j];
    }
    return 0;
}

int crypto_stream_chacha20_tinynacl(unsigned char *c, unsigned long long l, const unsigned char *n, const unsigned char *k) {

    long long j;

    for (j = 0; j < l; ++j) c[j] = 0;
    return crypto_stream_chacha20_tinynacl_xor(c, c, l, n, k);
}
