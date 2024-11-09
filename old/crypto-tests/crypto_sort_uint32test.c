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

int main(void) {

    test_321();
    test_i();
    _exit(0);
}
