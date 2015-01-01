/*
20140302
Jan Mojzis
Public domain.
*/

/*
XXX TODO
- check if byte_isequal is constant-time
*/

#include <stdio.h>
#include "fail.h"
#include "byte.h"

int main(void) {

    unsigned char b1[32];
    unsigned char b2[32];
    long long i, j;
    char *x = "abcdefgh";
    char y[8];

    /* byte_zero test */
    for (i = 0; i < 32; ++i) {
        for (j = 0; j < sizeof b1; ++j) b1[j] = 0;
        for (j = 0; j < sizeof b2; ++j) b2[j] = 0;
        b1[i] = 0xff; b2[i] = 0xff;
        byte_zero(b1, sizeof b1);

        if (byte_isequal(b1, 32, b2) != 0) fail("byte_zero failure");
    }

    /* byte_isequal test */
    for (i = 0; i < sizeof b1; ++i) b1[i] = 0xff;
    for (i = 0; i < sizeof b2; ++i) b2[i] = 0xff;

    if (byte_isequal(b1, 32, b2) == 0) fail("byte_isequal failure");

    for (i = 0; i < 32; ++i) {
        b1[i] = 0;
        if (byte_isequal(b1, 32, b2) != 0) fail("byte_isequal failure");
        b1[i] = 0xff;
    }

    /* byte_copy test */
    for (i = 0; i < sizeof b1; ++i) b1[i] = 0xff;
    for (i = 0; i < sizeof b2; ++i) b2[i] = 0x00;

    byte_copy(b1, 32, b2);
    if (byte_isequal(b1, 32, b2) == 0) fail("byte_copy failure");
    
    for (i = 0; i < 32; ++i) {
        b1[i] = 0;
        byte_copy(b1, 32, b2);
        if (byte_isequal(b1, 32, b2) == 0) fail("byte_copy failure");
        b1[i] = 0xff;
    }

    byte_zero(y, 8);
    if (!byte_isequal(y, 8, "\0\0\0\0\0\0\0\0")) fail("byte_zero/byte_isequal failure");

    byte_copy(y, 8, x);
    if (!byte_isequal(y, 8, x)) fail("byte_copy/byte_isequal failure");

    byte_copy(y, 7, y + 1);
    if (!byte_isequal(y, 8, "bcdefghh")) fail("byte_copy/byte_isequal failure");

    byte_zero(y, 8);
    if (!byte_isequal(y, 8, "\0\0\0\0\0\0\0\0")) fail("byte_zero/byte_isequal failure");

    return 0;
}
