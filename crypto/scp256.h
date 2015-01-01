#ifndef _SCP256_H____
#define _SCP256_H____

extern void scp256_reduce64(unsigned char *);
extern void scp256_reduce32(unsigned char *);
extern void scp256_inv(unsigned char *);
extern void scp256_mul(unsigned char *, const unsigned char *, const unsigned char *);
extern void scp256_add(unsigned char *, const unsigned char *, const unsigned char *);

#endif

