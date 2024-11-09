/*
20141018
Jan Mojzis
Public domain.
*/

#include "misc.h"
#include "crypto_hash_sha512.h"

#define SPACESIZE 5232

static unsigned char space[SPACESIZE + 16];
static unsigned char buf[crypto_hash_sha512_BYTES + 16];

static unsigned char test_pseudorandom_checksum[32] = {
    0xa1, 0x57, 0x2f, 0x67, 0x19, 0xa6, 0x88, 0x2f, 
    0x54, 0xa5, 0xa4, 0x7d, 0xe2, 0xd3, 0xa3, 0xfd, 
    0xd1, 0x1a, 0x73, 0x58, 0x84, 0xc7, 0xb9, 0xfd, 
    0x7f, 0xa2, 0x33, 0xbf, 0x86, 0xf7, 0x15, 0xee
};

static void test_pseudorandom(void) {

    long long i, j;

    checksum_zero();
    i = 0;
    for (j = 0; j < SPACESIZE; j += 1 + j / 16) {
        pseudorandombytes(space + i, j);
        crypto_hash_sha512(buf + i, space + i, j);
        checksum(buf + i, crypto_hash_sha512_BYTES);
        ++i;
        i %= 16;
    }
    fail_whenbadchecksum(test_pseudorandom_checksum);
}

int main(void) {

    test_pseudorandom();
    _exit(0);
}
