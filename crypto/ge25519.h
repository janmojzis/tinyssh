#ifndef _GE25519_H____
#define _GE25519_H____

#include "fe.h"

typedef fe ge25519[4];          /* X, Y, Z, T */

extern void ge25519_tobytes(unsigned char *, ge25519);
extern int ge25519_frombytes_negate_vartime(ge25519, const unsigned char *);
extern void ge25519_add(ge25519, ge25519, ge25519);
extern void ge25519_scalarmult(ge25519, ge25519, const unsigned char *);
extern void ge25519_scalarmult_base(ge25519, const unsigned char *);

#endif
