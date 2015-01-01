#include "uint32_unpack_big.h"

/*
The 'uint32_unpack_big' function converts 4 bytes
in big-endian format into 32-bit unsigned integer.
*/
crypto_uint32 uint32_unpack_big(const unsigned char *x) {

    crypto_uint32 y = 0;
    long long i;

    for (i = 0; i < 4; ++i) y = (y << 8) | x[i];
    return y;
}
