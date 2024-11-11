/*
20141017
Jan Mojzis
Public domain.
*/

#include "misc.h"
#include "crypto_onetimeauth_poly1305.h"

#define SPACESIZE 5232

static unsigned char space[SPACESIZE + 16];
static unsigned char key[crypto_onetimeauth_poly1305_KEYBYTES + 16];
static unsigned char buf[crypto_onetimeauth_poly1305_BYTES + 16];

static unsigned char test_pseudorandom_checksum[32] = {
    0xaf, 0xe7, 0xbb, 0x79, 0x2a, 0xe5, 0x81, 0xee, 
    0xbc, 0x1f, 0x5b, 0x79, 0x96, 0xb7, 0x27, 0x1a, 
    0xc2, 0x74, 0x65, 0x0e, 0x1d, 0xb5, 0xb7, 0x06, 
    0x10, 0x42, 0x6a, 0x50, 0x57, 0x8b, 0xf1, 0xb0
};


static void test_pseudorandom(void) {

    long long i, j;

    checksum_zero();
    i = 0;
    for (j = 0; j < SPACESIZE; j += 1 + j / 16) {

        pseudorandombytes(space + i, j);
        pseudorandombytes(key + i, crypto_onetimeauth_poly1305_KEYBYTES);

        crypto_onetimeauth_poly1305(buf + i, space + i, j, key + i);
        checksum(buf + i, crypto_onetimeauth_poly1305_BYTES);
    
        if (crypto_onetimeauth_poly1305_verify(buf + i, space + i, j, key + i) != 0) {
            fail_printdata("m", space + i, j);
            fail_printdata("key", key + i, crypto_onetimeauth_poly1305_KEYBYTES);
            fail_printdata("a", buf + i, crypto_onetimeauth_poly1305_BYTES);
            fail("crypto_onetimeauth_poly1305_verify() failure");
        }
        ++i;
        i %= 16;
    }
    fail_whenbadchecksum(test_pseudorandom_checksum);
}


int main(void) {

    test_pseudorandom();

    _exit(0);
}
