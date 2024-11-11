#ifndef _RANDOMBYTES_H____
#define _RANDOMBYTES_H____

#include "haslibrandombytes.h"
#ifdef HASLIBRANDOMBYTES
#include <randombytes.h>
#else
extern void randombytes(void *, long long);
#endif

#endif
