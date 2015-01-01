/*
20140709
Jan Mojzis
Public domain.
*/

#include <unistd.h>
#include "misc.h"
#include "crypto_stream_chacha20.h"

unsigned char space[5232];
unsigned char k[crypto_stream_chacha20_KEYBYTES + 16];
unsigned char n[crypto_stream_chacha20_NONCEBYTES + 16];

unsigned char o[32] = {
    0x98, 0x43, 0x28, 0xe2, 0x76, 0xc2, 0xc3, 0xa1, 
    0xe8, 0xf3, 0x50, 0x98, 0x15, 0xc4, 0xea, 0xa6, 
    0x2c, 0x54, 0x87, 0x1b, 0x02, 0x4c, 0x61, 0xd1, 
    0x01, 0xd9, 0xf0, 0xb1, 0xd5, 0x21, 0xc4, 0x7a
};

void test_alignment(void) {

    long long i;

    for (i = 0; i < 16; ++i) {
        crypto_stream_chacha20_xor(space + i, space + i, sizeof space - i, n + i, k + i);
    }
}

void test_rand(void) {

    long long i, j;
    unsigned int u;

    pseudorandombytes(space, sizeof space);
    pseudorandombytes(k, sizeof k);
    pseudorandombytes(n, sizeof n);

    for (i = 0; i < sizeof space; i += 1 + i / 16) {
        u = 1;
        for (j = 0; j < 8; ++j) {
            u += (unsigned int) n[j];
            n[j] = u;
            u >>= 8;
        }
        crypto_stream_chacha20_xor(space, space, i, n, k);
    }
    checksum(space, sizeof space);
    fail_whenbadchecksum(o);
}


int main(void) {

    test_alignment();
    test_rand();
    _exit(0);
}
