#ifndef SUBPROCESS_H____
#define SUBPROCESS_H____

extern int subprocess_sign(unsigned char *, long long, const char *,
                           unsigned char *, long long);
extern int subprocess_auth(const char *, const char *, const char *);

#endif
