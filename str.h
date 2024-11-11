#ifndef _STR_H____
#define _STR_H____

extern long long str_len(const char *);
extern int str_start(const char *, const char *);
extern int str_equaln(const char *, long long, const char *);
extern int str_copyn(char *, long long, const char *);

#endif
