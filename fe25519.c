#include "uint32_pack.h"
#include "uint32_unpack.h"
#include "crypto_verify_32.h"
#include "cleanup.h"
#include "fe.h"
#include "fe25519.h"

/*
p = 2^255 - 19
*/
static const fe p = { 
    0xffffffed, 0xffffffff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff, 0xffffffff, 0x7fffffff
};

/*
p3 = 3 * p
*/
static const crypto_uint64 p3[8] = { 
    0x2ffffffc7ULL, 0x2fffffffdULL, 0x2fffffffdULL, 0x2fffffffdULL,
    0x2fffffffdULL, 0x2fffffffdULL, 0x2fffffffdULL, 0x17ffffffdULL
};

/*
reduction modulo p: 16 limbs -> 8 limbs
*/
static void fe25519_reducebig(fe o, fel t) {

    crypto_uint64 u = 0;
    long long i;

    for (i = 0; i < 7; ++i) { u += t[i] + 38ULL * t[i + 8]; t[i] = u & 0xffffffff; u >>= 32; }
    u += t[i] + 38ULL * t[i + 8]; t[i] = u & 0x7fffffff; u >>= 31;
    u *= 19ULL;
    for (i = 0; i < 8; ++i) { u += t[i]; o[i] = u & 0xffffffff; u >>= 32; }
}

/*
o = (a * b) % p
*/
void fe25519_mul(fe o, const fe a, const fe b) {

    fel t;

    fe_mul_(t, a, b);
    fe25519_reducebig(o, t);

    cleanup(t);
}

/*
o = (a ^ 2) % p
*/
void fe25519_sq(fe o, const fe a) {

    fel t;

    fe_sq_(t, a);
    fe25519_reducebig(o, t);

    cleanup(t);
}

/* 
o = (121666 * f) % p; 
*/
void fe25519_mul121666(fe o, const fe f) {

    crypto_uint64 u = 0;
    long long i;

    for (i = 0; i < 7; ++i) { u += (crypto_uint64)121666 * (crypto_uint64)f[i]; o[i] = u & 0xffffffff; u >>= 32; }
    u += (crypto_uint64)121666 * (crypto_uint64)f[i]; o[i] = u & 0x7fffffff; u >>= 31;
    u *= 19ULL;
    for (i = 0; i < 8; ++i) { u += o[i]; o[i] = u & 0xffffffff; u >>= 32; }
}

/*
o = (x + y) % p
*/
void fe25519_add(fe o, const fe x, const fe y) {

    crypto_uint64 u = 0;
    long long i;

    for (i = 0; i < 7; ++i) { u += (crypto_uint64)x[i] + (crypto_uint64)y[i]; o[i] = u & 0xffffffff; u >>= 32; }
    u += (crypto_uint64)x[i] + (crypto_uint64)y[i]; o[i] = u & 0x7fffffff; u >>= 31;
    u *= 19ULL;
    for (i = 0; i < 8; ++i) { u += o[i]; o[i] = u & 0xffffffff; u >>= 32; }
}

/*
o = (x - y) % p
*/
void fe25519_sub(fe o, const fe x, const fe y) {

    crypto_uint64 u = 0;
    long long i;

    for (i = 0; i < 7; ++i) { u += p3[i] - (crypto_uint64)y[i] + (crypto_uint64)x[i]; o[i] = u & 0xffffffff; u >>= 32; }
    u += p3[i] - (crypto_uint64)y[i] + (crypto_uint64)x[i]; o[i] = u & 0x7fffffff; u >>= 31;
    u *= 19ULL;
    for (i = 0; i < 8; ++i) { u += o[i]; o[i] = u & 0xffffffff; u >>= 32; }
}

/*
o = -x % p
*/
void fe25519_neg(fe o, const fe x) {

    fe t;

    fe_0(t);
    fe25519_sub(o, t, x);
}


/* 
o = (1 / z) % p
... using Fermat's Little Theorem
*/
void fe25519_inv(fe o, const fe z) {

    fe t0, t1, t2, t3;
    long long  i;

    fe25519_sq(t0, z); for (i = 1; i < 1; ++i) fe25519_sq(t0, t0);
    fe25519_sq(t1,t0); for (i = 1; i < 2; ++i) fe25519_sq(t1, t1);
    fe25519_mul(t1, z, t1);
    fe25519_mul(t0, t0, t1);
    fe25519_sq(t2, t0); for (i = 1; i < 1; ++i) fe25519_sq(t2, t2);
    fe25519_mul(t1, t1, t2);
    fe25519_sq(t2, t1); for (i = 1; i < 5; ++i) fe25519_sq(t2, t2);
    fe25519_mul(t1, t2, t1);
    fe25519_sq(t2, t1); for (i = 1; i < 10; ++i) fe25519_sq(t2, t2);
    fe25519_mul(t2, t2, t1);
    fe25519_sq(t3, t2); for (i = 1; i < 20; ++i) fe25519_sq(t3, t3);
    fe25519_mul(t2, t3, t2);
    fe25519_sq(t2, t2); for (i = 1; i < 10; ++i) fe25519_sq(t2, t2);
    fe25519_mul(t1, t2, t1);
    fe25519_sq(t2, t1); for (i = 1; i < 50; ++i) fe25519_sq(t2, t2);
    fe25519_mul(t2, t2, t1);
    fe25519_sq(t3, t2); for (i = 1; i < 100; ++i) fe25519_sq(t3, t3);
    fe25519_mul(t2, t3, t2);
    fe25519_sq(t2, t2); for (i = 1; i < 50; ++i) fe25519_sq(t2, t2);
    fe25519_mul(t1, t2, t1);
    fe25519_sq(t1, t1); for (i = 1; i < 5; ++i) fe25519_sq(t1, t1);
    fe25519_mul(o, t1, t0);

    cleanup(t0); cleanup(t1); cleanup(t2); cleanup(t3);
}

void fe25519_pow22523(fe out, const fe z) {

    fe t0, t1, t2;
    long long i;

    fe25519_sq(t0, z); for (i = 1; i < 1; ++i) fe25519_sq(t0, t0);
    fe25519_sq(t1, t0); for (i = 1; i < 2; ++i) fe25519_sq(t1, t1);
    fe25519_mul(t1, z, t1);
    fe25519_mul(t0, t0, t1);
    fe25519_sq(t0, t0); for (i = 1; i < 1; ++i) fe25519_sq(t0, t0);
    fe25519_mul(t0, t1, t0);
    fe25519_sq(t1, t0); for (i = 1; i < 5; ++i) fe25519_sq(t1, t1);
    fe25519_mul(t0, t1, t0);
    fe25519_sq(t1, t0); for (i = 1; i < 10; ++i) fe25519_sq(t1, t1);
    fe25519_mul(t1, t1, t0);
    fe25519_sq(t2, t1); for (i = 1; i < 20; ++i) fe25519_sq(t2, t2);
    fe25519_mul(t1, t2, t1);
    fe25519_sq(t1, t1); for (i = 1; i < 10; ++i) fe25519_sq(t1, t1);
    fe25519_mul(t0, t1, t0);
    fe25519_sq(t1, t0); for (i = 1; i < 50; ++i) fe25519_sq(t1, t1);
    fe25519_mul(t1, t1, t0);
    fe25519_sq(t2, t1); for (i = 1; i < 100; ++i) fe25519_sq(t2, t2);
    fe25519_mul(t1, t2, t1);
    fe25519_sq(t1, t1); for (i = 1; i < 50; ++i) fe25519_sq(t1, t1);
    fe25519_mul(t0, t1, t0);
    fe25519_sq(t0, t0); for (i = 1; i < 2; ++i) fe25519_sq(t0, t0);
    fe25519_mul(out, t0, z);

    cleanup(t0); cleanup(t1); cleanup(t2);
}

/*
converts field-element into byte-array
*/
void fe25519_tobytes(unsigned char *out, const fe in) {

    long long i;
    fe x;

    fe_copy(x, in);
    fe_reducesmall(x, p, 0);
    for (i = 0; i < 8; ++i) uint32_pack(out + 4 * i, x[i]);
    cleanup(x);
}

/*
converts byte-array into field-element
*/
void fe25519_frombytes(fe out, const unsigned char *in) {

    long long i;

    for (i = 0; i < 8; ++i) out[i] = uint32_unpack(in + 4 * i);
    out[7] &= 0x7fffffff;
}


/*
if (f == 0) return 0;
else return -1;
*/
static const unsigned char zero[32] = {0};
int fe25519_isnonzero(const fe f) {
    unsigned char s[32];
    int r;
    fe25519_tobytes(s, f);
    r = crypto_verify_32(s, zero);
    cleanup(s);
    return r;
}


/*
if (f >= 0) return 0;
else return -1;
*/
int fe25519_isnegative(const fe f) {
    unsigned char s[32];
    int r;
    fe25519_tobytes(s,f);
    r = s[0] & 1;
    cleanup(s);
    return r;
}
