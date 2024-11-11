#ifndef CLEANUP_H____
#define CLEANUP_H____

extern void cleanup_(void *, long long);
#define cleanup(x) cleanup_((x), sizeof(x))

#endif
