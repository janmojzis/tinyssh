#include "misc.h"
#include "crypto_uint32.h"
#include "crypto_sort_uint32.h"

static void test_321(void) {

    crypto_uint32 x[3] = {3, 2, 1};

    crypto_sort_uint32(x, 3);

    if (x[0] != 1) fail("crypto_sort_uint32() failure");
    if (x[1] != 2) fail("crypto_sort_uint32() failure");
    if (x[2] != 3) fail("crypto_sort_uint32() failure");
}

static void test_i(void) {

    crypto_uint32 x[5] = { (crypto_uint32) -1, (crypto_uint32) -2, 2, 1, 0};

    crypto_sort_uint32(x, 5);

    if (x[0] != 0) fail("crypto_sort_uint32() failure");
    if (x[1] != 1) fail("crypto_sort_uint32() failure");
    if (x[2] != 2) fail("crypto_sort_uint32() failure");
    if (x[3] != (crypto_uint32) -2) fail("crypto_sort_uint32() failure");
    if (x[4] != (crypto_uint32) -1) fail("crypto_sort_uint32() failure");
}

#define SPACESIZE 123

static crypto_uint32 space[SPACESIZE + 4];
static crypto_uint32 buf[SPACESIZE + 4];

static unsigned char test_pseudorandom_checksum[32] = {
    0xb6, 0x48, 0x52, 0x41, 0xd0, 0x19, 0xa7, 0x8c,
    0x61, 0x4d, 0x29, 0x9d, 0x1f, 0xda, 0xb9, 0xc5,
    0xbd, 0x47, 0xe6, 0x7f, 0xc2, 0xf3, 0x2e, 0xfd,
    0x32, 0x21, 0x7a, 0x7b, 0xbb, 0xec, 0x02, 0x08
};

static void test_pseudorandom(void) {

    long long i, j;

    checksum_zero();
    i = 0;
    for (j = 0; j < SPACESIZE; j += 1 + j / 4) {
        pseudorandombytes(space + i, sizeof(crypto_uint32) * j);
        crypto_sort_uint32(space + i, j);
        checksum(space + i, sizeof(crypto_uint32) * j);
        ++i;
        i %= 4;
    }
    fail_whenbadchecksum(test_pseudorandom_checksum);
}

int main(void) {

    test_321();
    test_i();
    test_pseudorandom();
    _exit(0);
}
