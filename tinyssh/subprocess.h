#ifndef _SUBPROCESS_H____
#define _SUBPROCESS_H____

#include "buf.h"

extern int subprocess_sign(unsigned char *, long long, const char *, unsigned char *, long long);
extern int subprocess_auth(const char *, const char *, const char *);

#endif
