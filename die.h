#ifndef DIE_H____
#define DIE_H____

extern void die_usage(const char *);
extern void die_fatal_(const char *, unsigned long long, const char *,
                       const char *, const char *);
#define die_fatal(a, b, cc) die_fatal_(__FILE__, __LINE__, (a), (b), (cc))

#endif
