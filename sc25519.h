#ifndef _SC25519_H____
#define _SC25519_H____

extern void sc25519_reduce(unsigned char *);
extern void sc25519_muladd(unsigned char *, const unsigned char *, const unsigned char *, const unsigned char *);

#endif
