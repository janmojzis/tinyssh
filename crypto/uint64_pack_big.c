#include "uint64_pack_big.h"

/*
The 'uint64_pack_big' function converts 64-bit unsigned
integer into 8 bytes stored in big-endian format
*/
void uint64_pack_big(unsigned char *y, crypto_uint64 x) {

    long long i;

    for (i = 7; i >= 0; --i) { y[i] = x; x >>= 8; }
}
