/*
20250126
*/
/*
Based on poly1305-donna (https://github.com/floodyberry/poly1305-donna)
- modified for NaCl API
- switched to cryptoint
*/

#include "crypto_onetimeauth_poly1305.h"
#include "haslib1305.h"
#ifndef HASLIB1305

#include "crypto_int16.h"
#include "crypto_uint32.h"
#include "crypto_uint64.h"

/* clang-format off */
int crypto_onetimeauth_poly1305_tinyssh(unsigned char *o,
                                        const unsigned char *m,
                                        unsigned long long n,
                                        const unsigned char *k) {

    crypto_uint32 h0, h1, h2, h3, h4;
    crypto_uint32 r0, r1, r2, r3, r4;
    crypto_uint32     s1, s2, s3, s4;
    crypto_uint64 d0, d1, d2, d3, d4;
    crypto_uint32 c, mask;
    crypto_uint64 f;
    crypto_uint64 i;


    /* r &= 0xffffffc0ffffffc0ffffffc0fffffff */
    r0 = (crypto_uint32_load(k +  0)     ) & 0x3ffffff;
    r1 = (crypto_uint32_load(k +  3) >> 2) & 0x3ffff03;
    r2 = (crypto_uint32_load(k +  6) >> 4) & 0x3ffc0ff;
    r3 = (crypto_uint32_load(k +  9) >> 6) & 0x3f03fff;
    r4 = (crypto_uint32_load(k + 12) >> 8) & 0x00fffff;

    s1 = r1 * 5;
    s2 = r2 * 5;
    s3 = r3 * 5;
    s4 = r4 * 5;

    /* h = 0 */
    h0 = h1 = h2 = h3 = h4 = 0;

    while (n > 0) {
        /* h += m[i] */
        if (n >= 16) {
            h0 += (crypto_uint32_load(m     )     ) & 0x3ffffff;
            h1 += (crypto_uint32_load(m +  3) >> 2) & 0x3ffffff;
            h2 += (crypto_uint32_load(m +  6) >> 4) & 0x3ffffff;
            h3 += (crypto_uint32_load(m +  9) >> 6) & 0x3ffffff;
            h4 += (crypto_uint32_load(m + 12) >> 8) | 16777216;
            m += 16;
            n -= 16;
        }
        else {
            unsigned char mm[16];
            for (i = 0; i < 16; ++i) mm[i] = 0;
            for (i = 0; i <  n; ++i) mm[i] = m[i];
            mm[i] = 1;
            h0 += (crypto_uint32_load(mm     )     ) & 0x3ffffff;
            h1 += (crypto_uint32_load(mm +  3) >> 2) & 0x3ffffff;
            h2 += (crypto_uint32_load(mm +  6) >> 4) & 0x3ffffff;
            h3 += (crypto_uint32_load(mm +  9) >> 6) & 0x3ffffff;
            h4 += (crypto_uint32_load(mm + 12) >> 8);
            n = 0;
        }

        /* h *= r */
        d0 = ((crypto_uint64)h0 * r0) + ((crypto_uint64)h1 * s4) + ((crypto_uint64)h2 * s3) + ((crypto_uint64)h3 * s2) + ((crypto_uint64)h4 * s1);
        d1 = ((crypto_uint64)h0 * r1) + ((crypto_uint64)h1 * r0) + ((crypto_uint64)h2 * s4) + ((crypto_uint64)h3 * s3) + ((crypto_uint64)h4 * s2);
        d2 = ((crypto_uint64)h0 * r2) + ((crypto_uint64)h1 * r1) + ((crypto_uint64)h2 * r0) + ((crypto_uint64)h3 * s4) + ((crypto_uint64)h4 * s3);
        d3 = ((crypto_uint64)h0 * r3) + ((crypto_uint64)h1 * r2) + ((crypto_uint64)h2 * r1) + ((crypto_uint64)h3 * r0) + ((crypto_uint64)h4 * s4);
        d4 = ((crypto_uint64)h0 * r4) + ((crypto_uint64)h1 * r3) + ((crypto_uint64)h2 * r2) + ((crypto_uint64)h3 * r1) + ((crypto_uint64)h4 * r0);

        /* (partial) h %= p */
                      c = (crypto_uint32)(d0 >> 26); h0 = (crypto_uint32)d0 & 0x3ffffff;
        d1 += c;      c = (crypto_uint32)(d1 >> 26); h1 = (crypto_uint32)d1 & 0x3ffffff;
        d2 += c;      c = (crypto_uint32)(d2 >> 26); h2 = (crypto_uint32)d2 & 0x3ffffff;
        d3 += c;      c = (crypto_uint32)(d3 >> 26); h3 = (crypto_uint32)d3 & 0x3ffffff;
        d4 += c;      c = (crypto_uint32)(d4 >> 26); h4 = (crypto_uint32)d4 & 0x3ffffff;
        h0 += c * 5;  c =                (h0 >> 26); h0 =                h0 & 0x3ffffff;
        h1 += c;
    }


    /* fully carry h */
                 c = h1 >> 26; h1 = h1 & 0x3ffffff;
    h2 +=     c; c = h2 >> 26; h2 = h2 & 0x3ffffff;
    h3 +=     c; c = h3 >> 26; h3 = h3 & 0x3ffffff;
    h4 +=     c; c = h4 >> 26; h4 = h4 & 0x3ffffff;
    h0 += c * 5; c = h0 >> 26; h0 = h0 & 0x3ffffff;
    h1 +=     c;

    /* compute h + -p */
    r0 = h0 + 5; c = r0 >> 26; r0 &= 0x3ffffff;
    r1 = h1 + c; c = r1 >> 26; r1 &= 0x3ffffff;
    r2 = h2 + c; c = r2 >> 26; r2 &= 0x3ffffff;
    r3 = h3 + c; c = r3 >> 26; r3 &= 0x3ffffff;
    r4 = h4 + c - (1 << 26);

    /* select h if h < p, or h + -p if h >= p */
    mask = crypto_uint32_topbit_01(r4);
    mask -= 1;
    r0 ^= h0;
    r1 ^= h1;
    r2 ^= h2;
    r3 ^= h3;
    r4 ^= h4;
    h0 ^= mask & r0;
    h1 ^= mask & r1;
    h2 ^= mask & r2;
    h3 ^= mask & r3;
    h4 ^= mask & r4;

    /* h = h % (2^128) */
    h0 = ((h0      ) | (h1 << 26)) & 0xffffffff;
    h1 = ((h1 >>  6) | (h2 << 20)) & 0xffffffff;
    h2 = ((h2 >> 12) | (h3 << 14)) & 0xffffffff;
    h3 = ((h3 >> 18) | (h4 <<  8)) & 0xffffffff;

    /* mac = (h + pad) % (2^128) */
    f = (crypto_uint64)h0 + crypto_uint32_load(k + 16)            ; h0 = (crypto_uint32)f;
    f = (crypto_uint64)h1 + crypto_uint32_load(k + 20) + (f >> 32); h1 = (crypto_uint32)f;
    f = (crypto_uint64)h2 + crypto_uint32_load(k + 24) + (f >> 32); h2 = (crypto_uint32)f;
    f = (crypto_uint64)h3 + crypto_uint32_load(k + 28) + (f >> 32); h3 = (crypto_uint32)f;

    crypto_uint32_store(o +  0, h0);
    crypto_uint32_store(o +  4, h1);
    crypto_uint32_store(o +  8, h2);
    crypto_uint32_store(o + 12, h3);

    return 0;
}

int crypto_onetimeauth_poly1305_tinyssh_verify(const unsigned char *h,
                                               const unsigned char *in,
                                               unsigned long long l,
                                               const unsigned char *k) {

    unsigned char correct[16];
    crypto_int16 d = 0;
    long long i;

    crypto_onetimeauth_poly1305(correct, in, l, k);

    for (i = 0; i < 16; ++i) d |= correct[i] ^ h[i];
    return crypto_int16_nonzero_mask(d);
}
/* clang-format on */
#endif
