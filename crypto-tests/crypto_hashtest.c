/*
20141019
Jan Mojzis
Public domain.
*/

#include "misc.h"
#include "crypto_hash.h"

#define SPACESIZE 5232

static unsigned char space[SPACESIZE + 16];
static unsigned char buf[crypto_hash_BYTES + 16];

static unsigned char test_pseudorandom_checksumsha2[32] = {
    0xa1, 0x57, 0x2f, 0x67, 0x19, 0xa6, 0x88, 0x2f, 
    0x54, 0xa5, 0xa4, 0x7d, 0xe2, 0xd3, 0xa3, 0xfd, 
    0xd1, 0x1a, 0x73, 0x58, 0x84, 0xc7, 0xb9, 0xfd, 
    0x7f, 0xa2, 0x33, 0xbf, 0x86, 0xf7, 0x15, 0xee
};

static unsigned char test_pseudorandom_checksumsha3[32] = {
    0xcf, 0x1c, 0x9a, 0xc0, 0x02, 0x06, 0x8e, 0x98,
    0xe5, 0x82, 0x65, 0x84, 0x77, 0x41, 0x0a, 0x9f,
    0xbf, 0xba, 0x58, 0xa7, 0xcb, 0x50, 0x97, 0x30,
    0x8c, 0x0b, 0xb1, 0xcc, 0xcc, 0xc4, 0xf4, 0xca
};


static void test_pseudorandom(void) {

    long long i, j;

    checksum_zero();
    i = 0;
    for (j = 0; j < SPACESIZE; j += 1 + j / 16) {
        pseudorandombytes(space + i, j);
        crypto_hash(buf + i, space + i, j);
        checksum(buf + i, crypto_hash_BYTES);
        ++i;
        i %= 16;
    }
    if (!checksum_check((test_pseudorandom_checksumsha2)) && !checksum_check((test_pseudorandom_checksumsha3))) {
        fail_whenbadchecksum(test_pseudorandom_checksumsha3);
    }

}

int main(void) {

    test_pseudorandom();
    _exit(0);
}
