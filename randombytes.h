#ifndef _RANDOMBYTES_H____
#define _RANDOMBYTES_H____

extern void randombytes(unsigned char *, unsigned long long);

#ifndef randombytes_implementation
#define randombytes_implementation "tinyssh"
#endif

#endif
