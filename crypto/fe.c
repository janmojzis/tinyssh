/*
20140918
Jan Mojzis
Public domain.
*/

#include "fe.h"

/*
o = 0
*/
void fe_0(fe o) {

    long long i;
    for (i = 0; i < 8; ++i) o[i] = 0;
}

/*
o = 1
*/
void fe_1(fe o) {

    fe_0(o);
    o[0] = 1;
}

/*
o = x
*/
void fe_copy(fe o, const fe x) {

    long long i;
    for (i = 0; i < 8; ++i) o[i] = x[i];
}

/*
if (b) swap(f, g)
*/
void fe_cswap(fe f, fe g, crypto_uint32 b) {

    long long i;
    fe t;

    b = -b;

    for (i = 0; i < 8; ++i) t[i] = b & (f[i] ^ g[i]);
    for (i = 0; i < 8; ++i) f[i] ^= t[i];
    for (i = 0; i < 8; ++i) g[i] ^= t[i];
    fe_0(t);
}

/*
if (b) f = g
*/
void fe_cmov(fe f, const fe g, crypto_uint32 b) {

    long long i;
    fe t;

    b = -b;

    for (i = 0; i < 8; ++i) t[i] = b & (f[i] ^ g[i]);
    for (i = 0; i < 8; ++i) f[i] ^= t[i];
    fe_0(t);
}


/* 
o = a * b
*/
/*
Implementation note: fe_mul_() is unrolled version of:
void fe_mul_(fel o, const fe a, const fe b) {

    crypto_uint64 u;
    long long i, j;

    for (i = 0; i < 16; ++i) o[i] = 0;
    for (i = 0; i <  8; ++i) for (j = 0; j < 8; ++j) {
        u = (crypto_uint64)a[i] * (crypto_uint64)b[j];
        o[i + j    ] += u & 0xffffffff;
        o[i + j + 1] += u >> 32;
    }
}
*/
#define M(i, j) u = (crypto_uint64)a[i] * (crypto_uint64)b[j]; \
                o[i + j    ] += u & 0xffffffff; \
                o[i + j + 1] += u >> 32;
void fe_mul_(fel o, const fe a, const fe b) {

    crypto_uint64 u;
    long long i;

    for (i = 0; i < 16; ++i) o[i] = 0;

    M(0, 0); M(0, 1); M(0, 2); M(0, 3); M(0, 4); M(0, 5); M(0, 6); M(0, 7); 
    M(1, 0); M(1, 1); M(1, 2); M(1, 3); M(1, 4); M(1, 5); M(1, 6); M(1, 7); 
    M(2, 0); M(2, 1); M(2, 2); M(2, 3); M(2, 4); M(2, 5); M(2, 6); M(2, 7); 
    M(3, 0); M(3, 1); M(3, 2); M(3, 3); M(3, 4); M(3, 5); M(3, 6); M(3, 7); 
    M(4, 0); M(4, 1); M(4, 2); M(4, 3); M(4, 4); M(4, 5); M(4, 6); M(4, 7); 
    M(5, 0); M(5, 1); M(5, 2); M(5, 3); M(5, 4); M(5, 5); M(5, 6); M(5, 7); 
    M(6, 0); M(6, 1); M(6, 2); M(6, 3); M(6, 4); M(6, 5); M(6, 6); M(6, 7); 
    M(7, 0); M(7, 1); M(7, 2); M(7, 3); M(7, 4); M(7, 5); M(7, 6); M(7, 7);
}

/* 
o = x ^ 2 
*/
/*
Implementation note: fe_sq_() is unrolled version of:
void fe_sq_(fel o, const fe a) {

    crypto_uint64 u;
    long long i, j;

    for (i = 0; i < 16; ++i) o[i] = 0;
    for (i = 0; i <  8; ++i) for (j = i + 1; j < 8; ++j) {
        u = (crypto_uint64)a[i] * (crypto_uint64)a[j];
        o[i + j    ] += 2 * (u & 0xffffffff);
        o[i + j + 1] += 2 * (u >> 32);
    }
    for (i = 0; i <  8; ++i) {
        u = (crypto_uint64)a[i] * (crypto_uint64)a[i];
        o[2 * i    ] +=     (u & 0xffffffff);
        o[2 * i + 1] +=     (u >> 32);
    }
}
*/
#define M2(i, j) u = (crypto_uint64)a[i] * (crypto_uint64)a[j]; \
                 o[i + j    ] += 2 * (u & 0xffffffff); \
                 o[i + j + 1] += 2 * (u >> 32);
#define SQ(i)    u = (crypto_uint64)a[i] * (crypto_uint64)a[i]; \
                 o[2 * i    ] +=     (u & 0xffffffff); \
                 o[2 * i + 1] +=     (u >> 32)
void fe_sq_(fel o, const fe a) {

    crypto_uint64 u;
    long long i;

    for (i = 0; i < 16; ++i) o[i] = 0;

    M2(0, 1); M2(0, 2); M2(0, 3); M2(0, 4); M2(0, 5); M2(0, 6); M2(0, 7); 
    M2(1, 2); M2(1, 3); M2(1, 4); M2(1, 5); M2(1, 6); M2(1, 7); 
    M2(2, 3); M2(2, 4); M2(2, 5); M2(2, 6); M2(2, 7); 
    M2(3, 4); M2(3, 5); M2(3, 6); M2(3, 7); 
    M2(4, 5); M2(4, 6); M2(4, 7); 
    M2(5, 6); M2(5, 7); 
    M2(6, 7); 
    SQ(0); SQ(1); SQ(2); SQ(3); SQ(4); SQ(5); SQ(6); SQ(7); 
}

/*
if (p < r) r -= p
*/
void fe_reducesmall(fe r, const fe p, const crypto_uint64 carry) {

    crypto_uint64 pb = 0, b;
    long long i;
    fe t;

    for (i = 0; i < 8; ++i) {
        pb += (crypto_uint64)p[i];
        b = (crypto_uint64)r[i] - pb; b >>= 63;
        t[i] = (crypto_uint64)r[i] - pb + (b << 32);
        pb = b;
    }
    b = carry - pb; b >>= 63;
    b -= 1;
    for (i = 0; i < 8; ++i) r[i] ^= b & (r[i] ^ t[i]);
    fe_0(t);
}
