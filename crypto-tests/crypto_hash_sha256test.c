/*
20141018
Jan Mojzis
Public domain.
*/

#include "misc.h"
#include "crypto_hash_sha256.h"

#define SPACESIZE 5232

static unsigned char space[SPACESIZE + 16];
static unsigned char buf[crypto_hash_sha256_BYTES + 16];

static unsigned char test_pseudorandom_checksum[32] = {
    0xbd, 0x62, 0xb1, 0x24, 0x12, 0xce, 0xc2, 0xd6, 
    0xc9, 0x81, 0x29, 0x7f, 0x83, 0x8a, 0xe5, 0x19, 
    0xfd, 0xd7, 0x26, 0x4f, 0x13, 0x7e, 0xd5, 0x18, 
    0x40, 0x53, 0x76, 0x16, 0xe4, 0x22, 0x48, 0x50
};

static void test_pseudorandom(void) {

    long long i, j;

    checksum_zero();
    i = 0;
    for (j = 0; j < SPACESIZE; j += 1 + j / 16) {
        pseudorandombytes(space + i, j);
        crypto_hash_sha256(buf + i, space + i, j);
        checksum(buf + i, crypto_hash_sha256_BYTES);
        ++i;
        i %= 16;
    }
    fail_whenbadchecksum(test_pseudorandom_checksum);
}

int main(void) {

    test_pseudorandom();
    _exit(0);
}
