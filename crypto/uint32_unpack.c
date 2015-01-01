#include "order.h"
#include "uint32_unpack.h"

/*
The 'uint32_unpack' function converts 4 bytes
in little-endian format into 32-bit unsigned integer.
*/
crypto_uint32 uint32_unpack(const unsigned char *x) {

#ifdef ORDER_LITTLEENDIAN
    return *(crypto_uint32 *)x;
#else
    crypto_uint32 y = 0;
    long long i;
    for (i = 3; i >= 0; --i) y = (y << 8) | x[i];
    return y;
#endif
}
