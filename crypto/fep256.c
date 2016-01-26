/*
20140918
Jan Mojzis
Public domain.
*/

#include "uint32_pack_big.h"
#include "uint32_unpack_big.h"
#include "crypto_verify_32.h"
#include "crypto_int64.h"
#include "cleanup.h"
#include "fe.h"
#include "fep256.h"


/* p = 2^256 - 2^224 + 2^192 + 2^96 - 1 */
static const fe p = {
     0xffffffff, 0xffffffff, 0xffffffff, 0x00000000,
     0x00000000, 0x00000000, 0x00000001, 0xffffffff
};

/* pl = 2^256 - p */
static const fe pl = {
     0x00000001, 0x00000000, 0x00000000, 0xffffffff,
     0xffffffff, 0xffffffff, 0xfffffffe, 0x00000000
};

/*
reduction modulo p: 9 limbs -> 8 limbs
note: 0 < x < 9p
*/
static void fep256_reduce(fe o, crypto_uint64 t[9]) {

    crypto_uint64 u;
    long long i;

    u = 0;
    for (i = 0; i < 9; ++i) { u += t[i]; t[i] = u & 0xffffffff; u >>= 32; }
    u = 0;
    for (i = 0; i < 8; ++i) { u += t[i] + t[8] * (crypto_uint64)pl[i]; o[i] = u & 0xffffffff; u >>= 32; }
    fe_reducesmall(o, p, u);
}

/*
reduction modulo p: 16 limbs -> 8 limbs
*/
static void fep256_reducebig(fe o, fel t) {

    crypto_uint64 u = 0;
    crypto_int64 x[9], c, i;

    for (i = 0; i < 16; ++i) { u += t[i]; t[i] = u & 0xffffffff; u >>= 32; }

    x[8] = 0;
    x[7] = t[7] + 2 * t[15]             + t[15] + t[ 8] - t[10] - t[11] - t[12] - t[13];
    x[6] = t[6] + 2 * t[14] + 2 * t[15] + t[14] + t[13] - t[ 8] - t[ 9]                ;
    x[5] = t[5] + 2 * t[13] + 2 * t[14]         + t[15] - t[10] - t[11]                ;
    x[4] = t[4] + 2 * t[12] + 2 * t[13]         + t[14] - t[ 9] - t[10]                ;
    x[3] = t[3] + 2 * t[11] + 2 * t[12]         + t[13] - t[15] - t[ 8] - t[ 9]        ;
    x[2] = t[2] +                       + t[10] + t[11] - t[13] - t[14] - t[15]        ;
    x[1] = t[1] +                       + t[ 9] + t[10] - t[12] - t[13] - t[14] - t[15];
    x[0] = t[0] +                       + t[ 8] + t[ 9] - t[11] - t[12] - t[13] - t[14];

    x[0] += x[8];
    x[3] -= x[8];
    x[6] -= x[8];
    x[7] += x[8];
    x[8] = 4;

    for (i = 0; i < 8; ++i) {
        x[i] += (4LL << 32) - (crypto_uint64)pl[i] * 4LL;
        c = x[i] >> 32;
        x[i + 1] += c - 4;
        x[i    ] -= c << 32;
    }
    fep256_reduce(o, (crypto_uint64 *)x);

    cleanup(x);
}

/*
o = (a * b) % p
*/
void fep256_mul(fe o, const fe a, const fe b) {

    fel t;

    fe_mul_(t, a, b);
    fep256_reducebig(o, t);

    cleanup(t);
}

/*
o = (a ^ 2) % p
*/
void fep256_sq(fe o, const fe a) {

    fel t;

    fe_sq_(t, a);
    fep256_reducebig(o, t);

    cleanup(t);
}

/* 
o = (x * y) % p; 
*/
void fep256_mulx_(fe o, const fe x, const crypto_uint64 y) {

    crypto_uint64 t[9];
    long long i;

    t[8] = 0;
    for (i = 0; i < 8; ++i) t[i] = (crypto_uint64)x[i] * y;
    fep256_reduce(o, t);

    cleanup(t);
}


/* 
o = (x * 2) % p; 
*/
void fep256_mul2(fe o, const fe x) {

    crypto_uint64 u = 0;
    long long i;

    for (i = 0; i < 8; ++i) { u += (crypto_uint64)x[i] * 2ULL; o[i] = u & 0xffffffff; u >>= 32; }
    fe_reducesmall(o, p, u);
}


/* 
o = (x + y) % p; 
*/
void fep256_add(fe o, const fe x, const fe y) {

    crypto_uint64 u = 0;
    long long i;

    for (i = 0; i < 8; ++i) { u += (crypto_uint64)x[i] + (crypto_uint64)y[i]; o[i] = u & 0xffffffff; u >>= 32; }
    fe_reducesmall(o, p, u);
}

/* 
o = (x - y) % p; 
*/
void fep256_sub(fe o, const fe x, const fe y) {

    crypto_uint64 a, b, pb, u;
    long long i;

    u = pb = 0;
    for (i = 0; i < 8; ++i) {
        pb += (crypto_uint64)y[i];
        a = (crypto_uint64)x[i] + (crypto_uint64)p[i];
        b = a - pb; b >>= 63;
        a += (b << 32) - pb;
        pb = b;
        u += a; o[i] = u & 0xffffffff; u >>= 32;
    }
    fe_reducesmall(o, p, u);
}

/*
o = -x % p
*/
void fep256_neg(fe o, const fe x) {

    fe t;

    fe_0(t);
    fep256_sub(o, t, x);
}


/* 
out = (1 / in) % p;
... using Fermat's Little Theorem
13 * mul, 255 * sq
*/
void fep256_inv(fe out, const fe in) {

    fe o, x2, x4, x8, x16, x32;
    long long i;

    fep256_sq(o, in);
    for (i = 0; i < 1 - 1; ++i) fep256_sq(o, o);
    fep256_mul(x2, o, in);
    fep256_sq(o, x2);
    for (i = 0; i < 2 - 1; ++i) fep256_sq(o, o);
    fep256_mul(x4, o, x2);
    fep256_sq(o, x4);
    for (i = 0; i < 4 - 1; ++i) fep256_sq(o, o);
    fep256_mul(x8, o, x4);
    fep256_sq(o, x8);
    for (i = 0; i < 8 - 1; ++i) fep256_sq(o, o);
    fep256_mul(x16, o, x8);
    fep256_sq(o, x16);
    for (i = 0; i < 16 - 1; ++i) fep256_sq(o, o);
    fep256_mul(x32, o, x16);
    fep256_sq(o, x32);
    for (i = 0; i < 32 - 1; ++i) fep256_sq(o, o);
    fep256_mul(o, o, in);
    for (i = 0; i < 128; ++i) fep256_sq(o, o);
    fep256_mul(o, o, x32);
    for (i = 0; i < 32; ++i) fep256_sq(o, o);
    fep256_mul(o, o, x32);
    for (i = 0; i < 16; ++i) fep256_sq(o, o);
    fep256_mul(o, o, x16);
    for (i = 0; i < 8; ++i) fep256_sq(o, o);
    fep256_mul(o, o, x8);
    for (i = 0; i < 4; ++i) fep256_sq(o, o);
    fep256_mul(o, o, x4);
    for (i = 0; i < 2; ++i) fep256_sq(o, o);
    fep256_mul(o, o, x2);
    for (i = 0; i < 2; ++i) fep256_sq(o, o);
    fep256_mul(out, o, in);

    cleanup(o); cleanup(x2); cleanup(x4);
    cleanup(x8); cleanup(x16); cleanup(x32);
}

/*
converts field-element into byte-array
*/
void fep256_tobytes(unsigned char *out, const fe in) {

    long long i;
    fe x;

    fe_copy(x, in);
    fe_reducesmall(x, p, 0);
    for (i = 0; i < 8; ++i) uint32_pack_big(out + 28 - 4 * i, x[i]);
    cleanup(x);
}

/*
converts byte-array into field-element
*/
void fep256_frombytes(fe out, const unsigned char *in) {

    long long i;

    for (i = 0; i < 8; ++i) out[i] = uint32_unpack_big(in + 28 - 4 * i);
    fe_reducesmall(out, p, 0);
}


/*
if (f == 0) return 0;
else return -1;
*/
static const unsigned char zero[32] = {0};
int fep256_isnonzero(const fe f) {

    unsigned char s[32];
    int r;

    fep256_tobytes(s, f);
    r = crypto_verify_32(s, zero);
    cleanup(s);
    return r;
}
