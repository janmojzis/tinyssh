#include "cleanup.h"
#include "fe25519.h"
#include "crypto_scalarmult_curve25519.h"

int crypto_scalarmult_curve25519_tinyssh(unsigned char *q, const unsigned char *n, const unsigned char *p) {

    unsigned char e[32];
    fe x1, x2, z2, x3, z3, tmp0, tmp1;
    long long i;
    unsigned int d = 0;
    int pos;
    crypto_uint32 swap, b;

    for (i = 0; i < 32; ++i) e[i] = n[i];
    e[0] &= 248;
    e[31] &= 127;
    e[31] |= 64;
    fe25519_frombytes(x1, p);
    fe_1(x2);
    fe_0(z2);
    fe_copy(x3, x1);
    fe_1(z3);

    swap = 0;
    for (pos = 254; pos >= 0; --pos) {
        b = e[pos / 8] >> (pos & 7);
        b &= 1;
        swap ^= b;
        fe_cswap(x2, x3, swap);
        fe_cswap(z2, z3, swap);
        swap = b;

        fe25519_sub(tmp0, x3, z3);
        fe25519_sub(tmp1, x2, z2);
        fe25519_add(x2, x2, z2);
        fe25519_add(z2, x3, z3);
        fe25519_mul(z3, tmp0, x2);
        fe25519_mul(z2, z2, tmp1);
        fe25519_sq(tmp0, tmp1);
        fe25519_sq(tmp1, x2);
        fe25519_add(x3, z3, z2);
        fe25519_sub(z2, z3, z2);
        fe25519_mul(x2, tmp1, tmp0);
        fe25519_sub(tmp1, tmp1, tmp0);
        fe25519_sq(z2, z2);
        fe25519_mul121666(z3, tmp1);
        fe25519_sq(x3, x3);
        fe25519_add(tmp0, tmp0, z3);
        fe25519_mul(z3, x1, z2);
        fe25519_mul(z2, tmp1, tmp0);
    }

    fe_cswap(x2, x3, swap);
    fe_cswap(z2, z3, swap);

    fe25519_inv(z2, z2);
    fe25519_mul(x2, x2, z2);
    fe25519_tobytes(q, x2);

    cleanup(e);
    cleanup(tmp0); cleanup(tmp1);
    cleanup(x1); cleanup(x2); cleanup(x3);
    cleanup(z2); cleanup(z3);

    for (i = 0; i < 32; ++i) d |= q[i];
    return -(1 & ((d - 1) >> 8));
}

static const unsigned char basepoint[32] = {9};

int crypto_scalarmult_curve25519_tinyssh_base(unsigned char *q, const unsigned char *n) {
    return crypto_scalarmult_curve25519_tinyssh(q, n, basepoint);
}
