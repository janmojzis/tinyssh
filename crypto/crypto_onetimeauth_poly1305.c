/*
20180104
*/
/*
Based on poly1305-donna (https://github.com/floodyberry/poly1305-opt/blob/master/extensions/poly1305_ref-32.c)
- modified for NaCl API
*/

#include "crypto_onetimeauth_poly1305.h"

static unsigned long unpack(const unsigned char *x) {
    return
        (unsigned long) (x[0])                  \
    | (((unsigned long) (x[1])) << 8)           \
    | (((unsigned long) (x[2])) << 16)          \
    | (((unsigned long) (x[3])) << 24);
}
static void pack(unsigned char *x, unsigned long u) {
    x[0] = u; u >>= 8;
    x[1] = u; u >>= 8;
    x[2] = u; u >>= 8;
    x[3] = u;
}

int crypto_onetimeauth_poly1305_tinyssh(unsigned char *o, const unsigned char *m, unsigned long long n, const unsigned char *k) {

    unsigned long h0, h1, h2, h3, h4;
    unsigned long r0, r1, r2, r3, r4;
    unsigned long     s1, s2, s3, s4;
    unsigned long long d0, d1, d2, d3, d4;
    unsigned long c, mask;
    unsigned long long f;
    long long i;


    /* r &= 0xffffffc0ffffffc0ffffffc0fffffff */
    r0 = (unpack(k +  0)     ) & 0x3ffffff;
    r1 = (unpack(k +  3) >> 2) & 0x3ffff03;
    r2 = (unpack(k +  6) >> 4) & 0x3ffc0ff;
    r3 = (unpack(k +  9) >> 6) & 0x3f03fff;
    r4 = (unpack(k + 12) >> 8) & 0x00fffff;

    s1 = r1 * 5;
    s2 = r2 * 5;
    s3 = r3 * 5;
    s4 = r4 * 5;

    /* h = 0 */
    h0 = h1 = h2 = h3 = h4 = 0;

    while ((long long)n > 0) {

        /* h += m[i] */
        if (n >= 16) {
            h0 += (unpack(m     )     ) & 0x3ffffff;
            h1 += (unpack(m +  3) >> 2) & 0x3ffffff;
            h2 += (unpack(m +  6) >> 4) & 0x3ffffff;
            h3 += (unpack(m +  9) >> 6) & 0x3ffffff;
            h4 += (unpack(m + 12) >> 8) | 16777216;
        }
        else {
            unsigned char mm[16];
            for (i = 0; i < 16; ++i) mm[i] = 0;
            for (i = 0; i <  n; ++i) mm[i] = m[i];
            mm[i] = 1;
            h0 += (unpack(mm     )     ) & 0x3ffffff;
            h1 += (unpack(mm +  3) >> 2) & 0x3ffffff;
            h2 += (unpack(mm +  6) >> 4) & 0x3ffffff;
            h3 += (unpack(mm +  9) >> 6) & 0x3ffffff;
            h4 += (unpack(mm + 12) >> 8);
        }

        /* h *= r */
        d0 = ((unsigned long long)h0 * r0) + ((unsigned long long)h1 * s4) + ((unsigned long long)h2 * s3) + ((unsigned long long)h3 * s2) + ((unsigned long long)h4 * s1);
        d1 = ((unsigned long long)h0 * r1) + ((unsigned long long)h1 * r0) + ((unsigned long long)h2 * s4) + ((unsigned long long)h3 * s3) + ((unsigned long long)h4 * s2);
        d2 = ((unsigned long long)h0 * r2) + ((unsigned long long)h1 * r1) + ((unsigned long long)h2 * r0) + ((unsigned long long)h3 * s4) + ((unsigned long long)h4 * s3);
        d3 = ((unsigned long long)h0 * r3) + ((unsigned long long)h1 * r2) + ((unsigned long long)h2 * r1) + ((unsigned long long)h3 * r0) + ((unsigned long long)h4 * s4);
        d4 = ((unsigned long long)h0 * r4) + ((unsigned long long)h1 * r3) + ((unsigned long long)h2 * r2) + ((unsigned long long)h3 * r1) + ((unsigned long long)h4 * r0);

        /* (partial) h %= p */
                      c = (unsigned long)(d0 >> 26); h0 = (unsigned long)d0 & 0x3ffffff;
        d1 += c;      c = (unsigned long)(d1 >> 26); h1 = (unsigned long)d1 & 0x3ffffff;
        d2 += c;      c = (unsigned long)(d2 >> 26); h2 = (unsigned long)d2 & 0x3ffffff;
        d3 += c;      c = (unsigned long)(d3 >> 26); h3 = (unsigned long)d3 & 0x3ffffff;
        d4 += c;      c = (unsigned long)(d4 >> 26); h4 = (unsigned long)d4 & 0x3ffffff;
        h0 += c * 5;  c =                (h0 >> 26); h0 =                h0 & 0x3ffffff;
        h1 += c;

        m += 16;
        n -= 16;
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
    mask = (r4 >> ((sizeof(unsigned long) * 8) - 1)) - 1;
    r0 &= mask;
    r1 &= mask;
    r2 &= mask;
    r3 &= mask;
    r4 &= mask;
    mask = ~mask;
    h0 = (h0 & mask) | r0;
    h1 = (h1 & mask) | r1;
    h2 = (h2 & mask) | r2;
    h3 = (h3 & mask) | r3;
    h4 = (h4 & mask) | r4;

    /* h = h % (2^128) */
    h0 = ((h0      ) | (h1 << 26)) & 0xffffffff;
    h1 = ((h1 >>  6) | (h2 << 20)) & 0xffffffff;
    h2 = ((h2 >> 12) | (h3 << 14)) & 0xffffffff;
    h3 = ((h3 >> 18) | (h4 <<  8)) & 0xffffffff;

    /* mac = (h + pad) % (2^128) */
    f = (unsigned long long)h0 + unpack(k + 16)            ; h0 = (unsigned long)f;
    f = (unsigned long long)h1 + unpack(k + 20) + (f >> 32); h1 = (unsigned long)f;
    f = (unsigned long long)h2 + unpack(k + 24) + (f >> 32); h2 = (unsigned long)f;
    f = (unsigned long long)h3 + unpack(k + 28) + (f >> 32); h3 = (unsigned long)f;

    pack(o +  0, h0);
    pack(o +  4, h1);
    pack(o +  8, h2);
    pack(o + 12, h3);

    return 0;
}

int crypto_onetimeauth_poly1305_tinyssh_verify(const unsigned char *h, const unsigned char *in, unsigned long long l, const unsigned char *k) {

    unsigned char correct[16];
    unsigned int d = 0;
    long long i;

    crypto_onetimeauth_poly1305(correct, in, l, k);

    for (i = 0; i < 16; ++i) d |= correct[i] ^ h[i];
    return (1 & ((d - 1) >> 8)) - 1;
}
