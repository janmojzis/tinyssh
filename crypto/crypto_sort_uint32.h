#ifndef crypto_sort_uint32_H
#define crypto_sort_uint32_H

extern void crypto_sort_uint32_tinynacl(void *, long long);

#define crypto_sort_uint32 crypto_sort_uint32_tinynacl
#define crypto_sort_uint32_IMPLEMENTATION "tinynacl"
#define crypto_sort_uint32_VERSION "-"

#endif
