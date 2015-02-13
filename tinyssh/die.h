#ifndef _DIE_H____
#define _DIE_H____

extern void die_usage(const char *);
extern void die_fatal_(const char *, unsigned long long, const char *, const char *, const char *);
#define die_fatal(a, b, c) die_fatal_(__FILE__, __LINE__, (a), (b), (c)) 

#endif
