#ifndef _CLEANUP_H____
#define _CLEANUP_H____

extern void cleanup_(void *, long long);
#define cleanup(x) cleanup_((x), sizeof(x))

#endif
