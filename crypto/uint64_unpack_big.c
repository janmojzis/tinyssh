#include "uint64_unpack_big.h"

/*
The 'uint64_unpack_big' function converts 8 bytes
in big-endian format into 64-bit unsigned integer.
*/
crypto_uint64 uint64_unpack_big(const unsigned char *x) {

    crypto_uint64 y = 0;
    long long i;

    for (i = 0; i < 8; ++i) y = (y << 8) | x[i];
    return y;
}
