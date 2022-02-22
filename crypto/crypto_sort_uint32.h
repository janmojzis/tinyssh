#ifndef crypto_sort_uint32_H
#define crypto_sort_uint32_H

extern void crypto_sort_uint32_tinyssh(void *, long long);

#define crypto_sort_uint32 crypto_sort_uint32_tinyssh
#define crypto_sort_uint32_IMPLEMENTATION "tinyssh"
#define crypto_sort_uint32_VERSION "-"

#endif
