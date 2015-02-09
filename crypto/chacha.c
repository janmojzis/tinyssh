/*
20140529
Jan Mojzis
Public domain.
*/

#include "cleanup.h"
#include "crypto_uint64.h"
#include "uint32_pack.h"
#include "uint32_unpack.h"
#include "chacha.h"

#define ROTATE(x, c) ((x) << (c)) | ((x) >> (32 - (c)))

#define QUARTERROUND(a,b,c,d) \
    x[a] += x[b]; x[d] = ROTATE(x[d] ^ x[a], 16); \
    x[c] += x[d]; x[b] = ROTATE(x[b] ^ x[c], 12); \
    x[a] += x[b]; x[d] = ROTATE(x[d] ^ x[a],  8); \
    x[c] += x[d]; x[b] = ROTATE(x[b] ^ x[c],  7);

static void chacha_core(unsigned char *out, crypto_uint32 *n, const crypto_uint32 *k, const crypto_uint32 *c, long long r) {

    long long i;

    crypto_uint32 x[16], y[16];

    for (i = 0; i < 4; ++i) {
        x[i +  0] = c[i    ];
        x[i +  4] = k[i    ];
        x[i +  8] = k[i + 4];
        x[i + 12] = n[i    ];
    }

    for (i = 0; i < 16; ++i) y[i] = x[i];

    for (i = r; i > 0; i -= 2) {
        QUARTERROUND( 0, 4,  8, 12)
        QUARTERROUND( 1, 5,  9, 13)
        QUARTERROUND( 2, 6, 10, 14)
        QUARTERROUND( 3, 7, 11, 15)
        QUARTERROUND( 0, 5, 10, 15)
        QUARTERROUND( 1, 6, 11, 12)
        QUARTERROUND( 2, 7,  8, 13)
        QUARTERROUND( 3, 4,  9, 14)
    }

    for (i = 0; i < 16; ++i) uint32_pack(out + 4 * i, x[i] + y[i]);
    cleanup(x); cleanup(y);
}

/* expand 32-byte k */
static const crypto_uint32 sigma[4] = { 0x61707865, 0x3320646E, 0x79622D32, 0x6B206574 };


int chacha_stream_xor(unsigned char *c, const unsigned char *m, unsigned long long l, const unsigned char *nn, const unsigned char *kk, long long r) {

    unsigned char x[64];
    crypto_uint32 k[8], n[4];
    long long i;
    crypto_uint64 u;

    if (!l) return 0;

    for (i = 0; i < 8; ++i) k[i    ] = uint32_unpack(kk + 4 * i);
    for (i = 0; i < 2; ++i) n[i    ] = 0;
    for (i = 0; i < 2; ++i) n[i + 2] = uint32_unpack(nn + 4 * i);

    while (l >= 64) {
        chacha_core(m ? x : c, n, k, sigma, r);
        if (m) for (i = 0; i < 64; ++i) c[i] = m[i] ^ x[i];

        u = 1;
        for (i = 0; i < 2; ++i) {
            u += (crypto_uint64)n[i];
            n[i] = u;
            u >>= 32;
        }

        l -= 64;
        c += 64;
        if (m) m += 64;
    }
    if (l) {
        chacha_core(x, n, k, sigma, r);
        for (i = 0; i < l; ++i) c[i] = (m ? m[i] : 0) ^ x[i];
    }
    cleanup(x); cleanup(k); cleanup(n);
    return 0;
}
