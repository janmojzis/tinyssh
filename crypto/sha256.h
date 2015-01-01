#ifndef _SHA256_H_____
#define _SHA256_H_____

extern int sha256_auth(unsigned char *, const unsigned char *, unsigned long long, const unsigned char *);
extern int sha256_hash(unsigned char *, const unsigned char *, unsigned long long);

#endif
