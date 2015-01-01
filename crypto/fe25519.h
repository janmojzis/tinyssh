#ifndef _FE25519_H____
#define _FE25519_H____

#include "fe.h"

extern void fe25519_mul(fe, const fe, const fe);
extern void fe25519_sq(fe, const fe);
extern void fe25519_add(fe, const fe, const fe);
extern void fe25519_mul121666(fe, const fe);
extern void fe25519_sub(fe, const fe, const fe);
extern void fe25519_neg(fe, const fe);
extern void fe25519_inv(fe, const fe);
extern void fe25519_pow22523(fe, const fe);

extern void fe25519_tobytes(unsigned char *, const fe);
extern void fe25519_frombytes(fe, const unsigned char *);

extern int fe25519_isnonzero(const fe);
extern int fe25519_isnegative(const fe);

#endif

