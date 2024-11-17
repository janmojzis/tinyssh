#ifndef RANDOMBYTES_H____
#define RANDOMBYTES_H____

#include "haslibrandombytes.h"
#ifdef HASLIBRANDOMBYTES
#include <randombytes.h>
#else
#define randombytes randombytes_internal_void_voidstar_longlong
#define randombytes_source randombytes_internal_source
extern void randombytes(void *, long long);
extern const char *randombytes_source(void);
#endif

#endif
