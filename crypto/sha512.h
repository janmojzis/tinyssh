#ifndef _SHA512_H____
#define _SHA512_H____

#include "crypto_uint64.h"

extern void sha512_hash(unsigned char *, const unsigned char *, unsigned long long, crypto_uint64 *);

#endif
