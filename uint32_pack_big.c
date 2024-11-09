#include "uint32_pack_big.h"

/*
The 'uint32_pack_big' function converts 32-bit unsigned
integer into 4 bytes stored in big-endian format
*/
void uint32_pack_big(unsigned char *y, crypto_uint32 x) {

    long long i;

    for (i = 3; i >= 0; --i) { y[i] = x; x >>= 8; }
}
