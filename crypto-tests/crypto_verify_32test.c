/*
20140167
Jan Mojzis
Public domain.
*/

#include "misc.h"
#include "crypto_verify_32.h"

static unsigned char b1[crypto_verify_32_BYTES + 16];
static unsigned char b2[crypto_verify_32_BYTES + 16];


static void test_alignment(void) {

    long long i;

    for (i = 0; i < sizeof b1; ++i) b1[i] = 0xff;
    for (i = 0; i < sizeof b2; ++i) b2[i] = 0xff;

    for (i = 0; i < 16; ++i) {
        if (crypto_verify_32(b1 + i, b2 + i) != 0) fail("crypto_verify_32 failure");
    }
}

static void test_verify(void) {

    long long i;

    for (i = 0; i < sizeof b1; ++i) b1[i] = 0xff;
    for (i = 0; i < sizeof b2; ++i) b2[i] = 0xff;

    if (crypto_verify_32(b1, b2) != 0) fail("crypto_verify_32 failure");

    for (i = 0; i < crypto_verify_32_BYTES; ++i) {
        b1[i] = 0;
        if (crypto_verify_32(b1, b2) == 0) fail("crypto_verify_32 failure");
        b1[i] = 0xff;
    }
}

int main(void) {

    test_alignment();
    test_verify();

    _exit(0);
}
