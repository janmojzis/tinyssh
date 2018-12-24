#ifndef _FE_H____
#define _FE_H____

#include "crypto_uint32.h"
#include "crypto_uint64.h"

typedef crypto_uint32 fe[8];
typedef crypto_uint64 fel[16];

extern void fe_0(fe);
extern void fe_1(fe);
extern void fe_copy(fe, const fe);
extern void fe_cswap(fe, fe, crypto_uint32);
extern void fe_cmov(fe, const fe, crypto_uint32);

extern void fe_mul_(fel, const fe, const fe);
extern void fe_sq_(fel, const fe);

extern void fe_reducesmall(fe, const fe, const crypto_uint64);

#endif
