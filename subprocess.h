#ifndef _SUBPROCESS_H____
#define _SUBPROCESS_H____

#include <sys/types.h>

extern int subprocess_sign(unsigned char *, long long, const char *, unsigned char *, long long);
extern int subprocess_auth(const char *, const char *, const char *);

extern int subprocess_auth_checkpath_(char *, long long, uid_t);
extern int subprocess_auth_authorizedkeys_(const char *, const char *, const char *, char *, long long);

#endif
