#ifndef _AES_H_____
#define _AES_H_____

extern int aes_expandkey256(unsigned char *, const unsigned char *);
extern int aes_expandkey128(unsigned char *, const unsigned char *);
extern void aes_core(unsigned char *, const unsigned char *, const unsigned char *, long long);
extern int aes_stream_xor(unsigned char *, const unsigned char *, unsigned long long, const unsigned char *, const unsigned char *, long long);

#endif
