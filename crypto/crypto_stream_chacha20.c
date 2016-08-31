/*
20160830
Jan Mojzis
Public domain.
*/

#include "crypto_uint32.h"
#include "crypto_uint64.h"
#include "order.h"
#include "crypto_stream_chacha20.h"

#ifdef ORDER_LITTLEENDIAN
#define pack(y, x) *(crypto_uint32 *)(y) = (x)
#define unpack(x) *(crypto_uint32 *)(x)
#else
static crypto_uint32 unpack(const unsigned char *x) {
    return
        (crypto_uint32) (x[0])                  \
    | (((crypto_uint32) (x[1])) << 8)           \
    | (((crypto_uint32) (x[2])) << 16)          \
    | (((crypto_uint32) (x[3])) << 24);
}
static void pack(unsigned char *x, crypto_uint32 u) {
    x[0] = u; u >>= 8;
    x[1] = u; u >>= 8;
    x[2] = u; u >>= 8;
    x[3] = u;
}
#endif

#define ROTATE(x, c) ((x) << (c)) | ((x) >> (32 - (c)))

#define QUARTERROUND(a,b,c,d)                   \
    a += b; d = ROTATE(d ^ a, 16);              \
    c += d; b = ROTATE(b ^ c, 12);              \
    a += b; d = ROTATE(d ^ a,  8);              \
    c += d; b = ROTATE(b ^ c,  7);


#define xorblock(o,i)                           \
    x0  = s0;                                   \
    x1  = s1;                                   \
    x2  = s2;                                   \
    x3  = s3;                                   \
    x4  = k0;                                   \
    x5  = k1;                                   \
    x6  = k2;                                   \
    x7  = k3;                                   \
    x8  = k4;                                   \
    x9  = k5;                                   \
    x10 = k6;                                   \
    x11 = k7;                                   \
    x12 = n0;                                   \
    x13 = n1;                                   \
    x14 = n2;                                   \
    x15 = n3;                                   \
                                                \
    QUARTERROUND( x0, x4,  x8, x12)             \
    QUARTERROUND( x1, x5,  x9, x13)             \
    QUARTERROUND( x2, x6, x10, x14)             \
    QUARTERROUND( x3, x7, x11, x15)             \
    QUARTERROUND( x0, x5, x10, x15)             \
    QUARTERROUND( x1, x6, x11, x12)             \
    QUARTERROUND( x2, x7,  x8, x13)             \
    QUARTERROUND( x3, x4,  x9, x14)             \
    QUARTERROUND( x0, x4,  x8, x12)             \
    QUARTERROUND( x1, x5,  x9, x13)             \
    QUARTERROUND( x2, x6, x10, x14)             \
    QUARTERROUND( x3, x7, x11, x15)             \
    QUARTERROUND( x0, x5, x10, x15)             \
    QUARTERROUND( x1, x6, x11, x12)             \
    QUARTERROUND( x2, x7,  x8, x13)             \
    QUARTERROUND( x3, x4,  x9, x14)             \
    QUARTERROUND( x0, x4,  x8, x12)             \
    QUARTERROUND( x1, x5,  x9, x13)             \
    QUARTERROUND( x2, x6, x10, x14)             \
    QUARTERROUND( x3, x7, x11, x15)             \
    QUARTERROUND( x0, x5, x10, x15)             \
    QUARTERROUND( x1, x6, x11, x12)             \
    QUARTERROUND( x2, x7,  x8, x13)             \
    QUARTERROUND( x3, x4,  x9, x14)             \
    QUARTERROUND( x0, x4,  x8, x12)             \
    QUARTERROUND( x1, x5,  x9, x13)             \
    QUARTERROUND( x2, x6, x10, x14)             \
    QUARTERROUND( x3, x7, x11, x15)             \
    QUARTERROUND( x0, x5, x10, x15)             \
    QUARTERROUND( x1, x6, x11, x12)             \
    QUARTERROUND( x2, x7,  x8, x13)             \
    QUARTERROUND( x3, x4,  x9, x14)             \
    QUARTERROUND( x0, x4,  x8, x12)             \
    QUARTERROUND( x1, x5,  x9, x13)             \
    QUARTERROUND( x2, x6, x10, x14)             \
    QUARTERROUND( x3, x7, x11, x15)             \
    QUARTERROUND( x0, x5, x10, x15)             \
    QUARTERROUND( x1, x6, x11, x12)             \
    QUARTERROUND( x2, x7,  x8, x13)             \
    QUARTERROUND( x3, x4,  x9, x14)             \
    QUARTERROUND( x0, x4,  x8, x12)             \
    QUARTERROUND( x1, x5,  x9, x13)             \
    QUARTERROUND( x2, x6, x10, x14)             \
    QUARTERROUND( x3, x7, x11, x15)             \
    QUARTERROUND( x0, x5, x10, x15)             \
    QUARTERROUND( x1, x6, x11, x12)             \
    QUARTERROUND( x2, x7,  x8, x13)             \
    QUARTERROUND( x3, x4,  x9, x14)             \
    QUARTERROUND( x0, x4,  x8, x12)             \
    QUARTERROUND( x1, x5,  x9, x13)             \
    QUARTERROUND( x2, x6, x10, x14)             \
    QUARTERROUND( x3, x7, x11, x15)             \
    QUARTERROUND( x0, x5, x10, x15)             \
    QUARTERROUND( x1, x6, x11, x12)             \
    QUARTERROUND( x2, x7,  x8, x13)             \
    QUARTERROUND( x3, x4,  x9, x14)             \
    QUARTERROUND( x0, x4,  x8, x12)             \
    QUARTERROUND( x1, x5,  x9, x13)             \
    QUARTERROUND( x2, x6, x10, x14)             \
    QUARTERROUND( x3, x7, x11, x15)             \
    QUARTERROUND( x0, x5, x10, x15)             \
    QUARTERROUND( x1, x6, x11, x12)             \
    QUARTERROUND( x2, x7,  x8, x13)             \
    QUARTERROUND( x3, x4,  x9, x14)             \
    QUARTERROUND( x0, x4,  x8, x12)             \
    QUARTERROUND( x1, x5,  x9, x13)             \
    QUARTERROUND( x2, x6, x10, x14)             \
    QUARTERROUND( x3, x7, x11, x15)             \
    QUARTERROUND( x0, x5, x10, x15)             \
    QUARTERROUND( x1, x6, x11, x12)             \
    QUARTERROUND( x2, x7,  x8, x13)             \
    QUARTERROUND( x3, x4,  x9, x14)             \
    QUARTERROUND( x0, x4,  x8, x12)             \
    QUARTERROUND( x1, x5,  x9, x13)             \
    QUARTERROUND( x2, x6, x10, x14)             \
    QUARTERROUND( x3, x7, x11, x15)             \
    QUARTERROUND( x0, x5, x10, x15)             \
    QUARTERROUND( x1, x6, x11, x12)             \
    QUARTERROUND( x2, x7,  x8, x13)             \
    QUARTERROUND( x3, x4,  x9, x14)             \
                                                \
    pack(o +  0,  (x0 + s0) ^ unpack(i +  0));  \
    pack(o +  4,  (x1 + s1) ^ unpack(i +  4));  \
    pack(o +  8,  (x2 + s2) ^ unpack(i +  8));  \
    pack(o + 12,  (x3 + s3) ^ unpack(i + 12));  \
    pack(o + 16,  (x4 + k0) ^ unpack(i + 16));  \
    pack(o + 20,  (x5 + k1) ^ unpack(i + 20));  \
    pack(o + 24,  (x6 + k2) ^ unpack(i + 24));  \
    pack(o + 28,  (x7 + k3) ^ unpack(i + 28));  \
    pack(o + 32,  (x8 + k4) ^ unpack(i + 32));  \
    pack(o + 36,  (x9 + k5) ^ unpack(i + 36));  \
    pack(o + 40, (x10 + k6) ^ unpack(i + 40));  \
    pack(o + 44, (x11 + k7) ^ unpack(i + 44));  \
    pack(o + 48, (x12 + n0) ^ unpack(i + 48));  \
    pack(o + 52, (x13 + n1) ^ unpack(i + 52));  \
    pack(o + 56, (x14 + n2) ^ unpack(i + 56));  \
    pack(o + 60, (x15 + n3) ^ unpack(i + 60));


int crypto_stream_chacha20_tinynacl_xor(unsigned char *c, const unsigned char *m, unsigned long long l, const unsigned char *n, const unsigned char *k) {

    unsigned char bspace[64] = {0};
    unsigned char *b = bspace;
    long long j;

    crypto_uint32 x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15;
    crypto_uint32 k0 = unpack(k +  0);
    crypto_uint32 k1 = unpack(k +  4);
    crypto_uint32 k2 = unpack(k +  8);
    crypto_uint32 k3 = unpack(k + 12);
    crypto_uint32 k4 = unpack(k + 16);
    crypto_uint32 k5 = unpack(k + 20);
    crypto_uint32 k6 = unpack(k + 24);
    crypto_uint32 k7 = unpack(k + 28);
    crypto_uint32 n0 = 0;
    crypto_uint32 n1 = 0;
    crypto_uint32 n2 = unpack(n + 0);
    crypto_uint32 n3 = unpack(n + 4);
    crypto_uint32 s0 = 0x61707865;
    crypto_uint32 s1 = 0x3320646E;
    crypto_uint32 s2 = 0x79622D32;
    crypto_uint32 s3 = 0x6B206574;
    crypto_uint64 u;

    if (!l) return 0;

    while (l >= 64) {
        xorblock(c, m);

        u = 1;
        u += (crypto_uint64)n0;
        n0 = u;
        u >>= 32;
        u += (crypto_uint64)n1;
        n1 = u;

        l -= 64;
        c += 64;
        m += 64;
    }
    if (l) {
        for (j = 0; j < l; ++j) b[j] = m[j];
        xorblock(b, b);
        for (j = 0; j < l; ++j) c[j] = b[j];
    }
    return 0;
}

int crypto_stream_chacha20_tinynacl(unsigned char *c, unsigned long long l, const unsigned char *n, const unsigned char *k) {

    long long j;

    for (j = 0; j < l; ++j) c[j] = 0;
    return crypto_stream_chacha20_tinynacl_xor(c, c, l, n, k);
}
