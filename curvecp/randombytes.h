/* taken from nacl-20110221, removed C++ code */

/*
randombytes/devurandom.h version 20080713
D. J. Bernstein
Public domain.
*/

#ifndef randombytes_devurandom_H
#define randombytes_devurandom_H

extern void randombytes(unsigned char *,unsigned long long);

#ifndef randombytes_implementation
#define randombytes_implementation "tinyssh"
#endif

#endif
