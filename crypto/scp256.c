/*
20140918
Jan Mojzis
Public domain.
*/

#include "uint32_pack_big.h"
#include "uint32_unpack_big.h"
#include "fe.h"
#include "cleanup.h"
#include "scp256.h"


/* m = 0xffffffff00000000ffffffffffffffffbce6faada7179e84f3b9cac2fc632551L */
static const fe m = {
     0xfc632551, 0xf3b9cac2, 0xa7179e84, 0xbce6faad,
     0xffffffff, 0xffffffff, 0x00000000, 0xffffffff
};

/* m2 = bce6faada7179e84f3b9cac2fc63254f */
static const unsigned char m2[32] = {
    0x0b, 0x0c, 0x0e, 0x06, 0x0f, 0x0a, 0x0a, 0x0d,
    0x0a, 0x07, 0x01, 0x07, 0x09, 0x0e, 0x08, 0x04,
    0x0f, 0x03, 0x0b, 0x09, 0x0c, 0x0a, 0x0c, 0x02,
    0x0f, 0x0c, 0x06, 0x03, 0x02, 0x05, 0x04, 0x0f
};


/*
ml[0] = (2^256 * 2^0)  % m;
ml[1] = (2^256 * 2^32) % m;
ml[2] = (2^256 * 2^64) % m;
...
*/
static const fe ml[8] = {
    { 0x039cdaaf, 0x0c46353d, 0x58e8617b, 0x43190552, 0x00000000, 0x00000000, 0xffffffff, 0x00000000 },
    { 0x00000000, 0x039cdaaf, 0x0c46353d, 0x58e8617b, 0x43190552, 0x00000000, 0x00000000, 0xffffffff },
    { 0xfc632551, 0xf756a571, 0xb6faae70, 0x22159165, 0x9c0166cd, 0x43190552, 0x00000001, 0xfffffffe },
    { 0xf8c64aa2, 0xe7739585, 0x51cc17b8, 0x89b10547, 0x652e96b7, 0x9c0166cd, 0x43190554, 0xfffffffe },
    { 0xfc632551, 0xf01cf013, 0x9ad16947, 0x679b73e1, 0xccca0a99, 0x652e96b7, 0x9c0166ce, 0x43190552 },
    { 0x609a770e, 0x74d1cb7b, 0xadae2385, 0xa83f3999, 0x79318f1c, 0xccca0a99, 0x22159165, 0xdf1a6c20 },
    { 0x4a40048f, 0x227511de, 0x0a16ad07, 0x27f60529, 0xe2b8f21e, 0x79318f1c, 0xedaf9e78, 0x012ffd85 },
    { 0xb9bd8feb, 0x108e407a, 0x5c29d906, 0xecd00db8, 0x2845b238, 0xe2b8f21e, 0x78019197, 0xeedf9bfd }
};

/* 
r = x % m; 
*/
static void _reducebig(fe r, fel x) {

    crypto_uint64 i, j, u, t[10];

    u = 0;
    for (i = 0; i < 16; ++i) { u += x[i]; x[i] = u & 0xffffffff; u >>= 32; }

    /* 16 limbs -> 10 limbs  */
    for (i = 0; i < 8; ++i) t[i] = x[i];
    t[8] = t[9] = 0;

    for (j = 0; j < 8; ++j) for (i = 0; i < 8; ++i) {
        u = (crypto_uint64)ml[j][i] * x[j + 8];
        t[i    ] += u & 0xffffffff;
        t[i + 1] += u >> 32;
    }

    u = 0;
    for (i = 0; i < 10; ++i) { u += t[i]; t[i] = u & 0xffffffff; u >>= 32; }

    /* 10 limbs -> 9 limbs  */
    for (j = 0; j < 2; ++j) {
        u = 0;
        for (i = 0; i < 8; ++i) { u += t[i] + (crypto_uint32)ml[1][i] * t[9]; t[i] = u & 0xffffffff; u >>= 32; }
        u += t[8]; t[8] = u & 0xffffffff; u >>= 32; t[9] = u;
    }

    /* 9 limbs -> 8 limbs  */
    for (j = 0; j < 2; ++j) {
        u = 0;
        for (i = 0; i < 8; ++i) { u += t[i] + (crypto_uint32)ml[0][i] * t[8]; t[i] = u & 0xffffffff; u >>= 32; }
        t[8] = u;
    }

    /* final reduce */
    for (i = 0; i < 8; ++i) r[i] = t[i];
    fe_reducesmall(r, m, u);

    cleanup(t);
}

/*
o = (a * b) % m
*/
static void _mul(fe o, const fe a, const fe b) {

    fel t;

    fe_mul_(t, a, b);
    _reducebig(o, t);

    cleanup(t);
}

/*
o = (a ^ 2) % m
*/
static void _sq(fe o, const fe a) {

    fel t;

    fe_sq_(t, a);
    _reducebig(o, t);

    cleanup(t);
}


/* 
out = (a + b) % m; 
*/
static void _add(fe out, fe a, const fe b) {

    fel t;
    long long i;

    for (i = 0; i < 16; ++i) t[i] = 0;
    for (i = 0; i < 8; ++i) t[i] = (crypto_uint64)a[i] + (crypto_uint64)b[i];

    _reducebig(out, t);
    cleanup(t);
}


/* 
out = (1 / in) % m;
... using Fermat's Little Theorem
44 mul, 262 sq
*/
static void _inv(fe out, const fe in) {

    fe o, x2, x4, x8, x16, x32, t[16];
    long long i;

    _sq(o, in);
    for (i = 0; i < 1 - 1; ++i) _sq(o, o);
    _mul(x2, o, in);
    _sq(o, x2);
    for (i = 0; i < 2 - 1; ++i) _sq(o, o);
    _mul(x4, o, x2);
    _sq(o, x4);
    for (i = 0; i < 4 - 1; ++i) _sq(o, o);
    _mul(x8, o, x4);
    _sq(o, x8);
    for (i = 0; i < 8 - 1; ++i) _sq(o, o);
    _mul(x16, o, x8);
    _sq(o, x16);
    for (i = 0; i < 16 - 1; ++i) _sq(o, o);
    _mul(x32, o, x16);
    _sq(o, x32);
    for (i = 0; i < 32 - 1; ++i) _sq(o, o);
    for (i = 0; i < 32; ++i) _sq(o, o);
    _mul(o, o, x32);
    for (i = 0; i < 32; ++i) _sq(o, o);
    _mul(o, o, x32);

    fe_0(t[0]);
    fe_copy(t[1], in);
    _sq(t[2], t[1]);
    fe_copy(t[3], x2);
    for (i = 4; i < 15; ++i) {
        if ((i & 1) == 0) _sq(t[i], t[i / 2]);
        else _mul(t[i], t[i - 1], in);
    }
    fe_copy(t[15], x4);

    for (i = 0; i < 32; ++i) {
        _sq(o, o); _sq(o, o); _sq(o, o); _sq(o, o);
        _mul(o, o, t[m2[i]]);
    }
    fe_copy(out, o);

    cleanup(o); cleanup(t);
    cleanup(x2); cleanup(x4);
    cleanup(x8); cleanup(x16); cleanup(x32);
}

static void _tobytes(unsigned char *out, fe in) {

    long long i;

    fe_reducesmall(in, m, 0);
    for (i = 0; i < 8; ++i) uint32_pack_big(out + 28 - 4 * i, in[i]);
}

static void _frombytes(fe out, const unsigned char *in) {

    long long i;

    for (i = 0; i < 8; ++i) out[i] = uint32_unpack_big(in + 28 - 4 * i);
    fe_reducesmall(out, m, 0);
}

static void _from64bytes(fe out, const unsigned char *in) {

    long long i;
    fel t;

    for (i = 0; i < 16; ++i) t[i] = uint32_unpack_big(in + 60 - 4 * i);
    _reducebig(out, t);

    cleanup(t);
}

void scp256_reduce64(unsigned char *x) {

    fe a;
    long long i;

    _from64bytes(a, x);
    for (i = 0; i < 64; ++i) x[i] = 0;
    _tobytes(x, a);

    cleanup(a);
}

void scp256_reduce32(unsigned char *x) {

    fe a;

    _frombytes(a, x);
    fe_reducesmall(a, m, 0);
    _tobytes(x, a);

    cleanup(a);
}

void scp256_inv(unsigned char *x) {

    fe a;

    _frombytes(a, x);
    _inv(a, a);
    _tobytes(x, a);

    cleanup(a);
}


void scp256_mul(unsigned char *o, const unsigned char *x, const unsigned char *y) {

    fe a, b;

    _frombytes(a, x);
    _frombytes(b, y);
    _mul(a, a, b);
    _tobytes(o, a);

    cleanup(a); cleanup(b);
}

void scp256_add(unsigned char *o, const unsigned char *x, const unsigned char *y) {

    fe a, b;

    _frombytes(a, x);
    _frombytes(b, y);
    _add(a, a, b);
    _tobytes(o, a);

    cleanup(a); cleanup(b);
}
