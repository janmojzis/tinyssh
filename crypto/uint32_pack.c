#include "order.h"
#include "uint32_pack.h"

/*
The 'uint32_pack' function converts 32-bit unsigned
integer into 4 bytes stored in little-endian format
*/
void uint32_pack(unsigned char *y, crypto_uint32 x) {

#ifdef ORDER_LITTLEENDIAN
    *(crypto_uint32 *)y = x;
#else
    long long i;
    for (i = 0; i < 4; ++i) { y[i] = x; x >>= 8; }
#endif
}
