/*
20141018
Jan Mojzis
Public domain.
*/

#include "misc.h"
#include "crypto_hash_md5.h"

#define SPACESIZE 5232

static unsigned char space[SPACESIZE + 16];
static unsigned char buf[crypto_hash_md5_BYTES + 16];

static unsigned char test_pseudorandom_checksum[32] = {
    0xcc, 0xd1, 0x88, 0xe3, 0x45, 0x28, 0x6a, 0xd0, 
    0x45, 0x4d, 0x7c, 0x46, 0xe8, 0xe9, 0xed, 0xb9, 
    0xd0, 0x11, 0xae, 0xc9, 0x5c, 0xa5, 0x05, 0x9d, 
    0x17, 0xe0, 0xb9, 0x82, 0x11, 0xad, 0xd5, 0x50
};

static void test_pseudorandom(void) {

    long long i, j;

    checksum_zero();
    i = 0;
    for (j = 0; j < SPACESIZE; j += 1 + j / 16) {
        pseudorandombytes(space + i, j);
        crypto_hash_md5(buf + i, space + i, j);
        checksum(buf + i, crypto_hash_md5_BYTES);
        ++i;
        i %= 16;
    }
    fail_whenbadchecksum(test_pseudorandom_checksum);
}

static struct vector {
    const char *m;
    long long mlen;
    const char *h;
} vectors[] = {
    { "", 0, "\xd4\x1d\x8c\xd9\x8f\x00\xb2\x04\xe9\x80\x09\x98\xec\xf8\x42\x7e" },
    { "a", 1, "\x0c\xc1\x75\xb9\xc0\xf1\xb6\xa8\x31\xc3\x99\xe2\x69\x77\x26\x61" },
    { "abc", 3, "\x90\x01\x50\x98\x3c\xd2\x4f\xb0\xd6\x96\x3f\x7d\x28\xe1\x7f\x72" },
    { "message digest", 14, "\xf9\x6b\x69\x7d\x7c\xb7\x93\x8d\x52\x5a\x2f\x31\xaa\xf1\x61\xd0" },
    { "abcdefghijklmnopqrstuvwxyz", 26, "\xc3\xfc\xd3\xd7\x61\x92\xe4\x00\x7d\xfb\x49\x6c\xca\x67\xe1\x3b" },
    { "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789", 62, "\xd1\x74\xab\x98\xd2\x77\xd9\xf5\xa5\x61\x1c\x2c\x9f\x41\x9d\x9f" },
    { "12345678901234567890123456789012345678901234567890123456789012345678901234567890", 80, "\x57\xed\xf4\xa2\x2b\xe3\xc9\x55\xac\x49\xda\x2e\x21\x07\xb6\x7a" },
    { 0, 0, 0 }
};

static void test_vector(void) {

    long long i, j;
    unsigned char h[crypto_hash_md5_BYTES];

    for (i = 0; vectors[i].m; ++i) {
        crypto_hash_md5(h, (unsigned char *)vectors[i].m, vectors[i].mlen);
        for (j = 0; j < crypto_hash_md5_BYTES; ++j)
            if (h[j] != (unsigned char)vectors[i].h[j])
                fail("crypto_hash_md5() failure");
    }
}


int main(void) {

    test_vector();;
    test_pseudorandom();
    _exit(0);
}
