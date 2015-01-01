#ifndef _FEP256_H____
#define _FEP256_H____

#include "fe.h"

extern void fep256_mul(fe, const fe, const fe);
extern void fep256_sq(fe, const fe);
extern void fep256_mul2(fe, const fe);
extern void fep256_mulx_(fe, const fe, const crypto_uint64);
#define fep256_mul3(x, y) fep256_mulx_(x, y, 3)
#define fep256_mul6(x, y) fep256_mulx_(x, y, 6)
extern void fep256_inv(fe, const fe);
extern void fep256_add(fe, const fe, const fe);
extern void fep256_sub(fe, const fe, const fe);
extern void fep256_neg(fe, const fe);

extern void fep256_tobytes(unsigned char *, const fe);
extern void fep256_frombytes(fe, const unsigned char *);

extern int fep256_isnonzero(const fe);

#endif
