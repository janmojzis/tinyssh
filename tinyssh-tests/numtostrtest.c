/*
20140201
Jan Mojzis
Public domain.
*/

#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include "fail.h"
#include "numtostr.h"

static long long int128max(void) {

    long long x = 9223372036854775807LL;

    x <<= 32;
    x <<= 32;
    x += 9223372036854775807LL;
    x += 9223372036854775807LL;
    x += 1;

    return x;
}

static void test_bufferoverflow(void) {

    char buf[NUMTOSTR_LEN + 16];
    long long i, j;
    char ch[2] = {0x00, (char)0xff };
    long long  llmaxnum;
    long long llminnum;
    long long numlen = sizeof(long long);

    if (numlen == 16) {
        llmaxnum = int128max();
        llminnum = -llmaxnum - 1LL;
    }
    else if (numlen == 8) {
        llmaxnum = 9223372036854775807LL;
        llminnum = -llmaxnum - 1LL;
    }
    else {
        fail("unsupported size for 'num'");
    }

    if ((NUMTOSTR_LEN) < sizeof("-170141183460469231731687303715884105728")) fail("NUMTOSTR_LEN too small");

    for (j = 0; j < 2; ++j) {
        for (i = 0; i < sizeof buf; ++i) buf[i] = ch[j];
        numtostr(buf + 8, llminnum);
        for (i = 0; i < 8; ++i) if (buf[i] != ch[j]) fail("numtostr() writes before output");
        for (i = 0; i < 8; ++i) if (buf[i + NUMTOSTR_LEN + 8] != ch[j]) fail("numtostr() writes after output");
        for (i = 0; i < sizeof buf; ++i) buf[i] = ch[j];
        numtostr(buf + 8, llmaxnum);
        for (i = 0; i < 8; ++i) if (buf[i] != ch[j]) fail("numtostr() writes before output");
        for (i = 0; i < 8; ++i) if (buf[i + NUMTOSTR_LEN + 8] != ch[j]) fail("numtostr() writes after output");
    }
}

static void test_minmax(void) {

    const char *llmax;
    const char *llmin;
    long long llmaxnum;
    long long llminnum;
    char *x;
    long long numlen = sizeof(long long);

    if (numlen == 16) {
        llmax   = "170141183460469231731687303715884105727";
        llmin   = "-170141183460469231731687303715884105728";
        llmaxnum = int128max();
        llminnum = -llmaxnum - 1LL;
    }
    else if (numlen == 8) {
        llmax   = "9223372036854775807";
        llmin   = "-9223372036854775808";
        llmaxnum = 9223372036854775807LL;
        llminnum = -llmaxnum - 1LL;
    }
    else {
        fail("unsupported size for 'num'");
    }

    x = numtostr(0, llmaxnum);
    if (strcmp(x, llmax)) fail("numtostr() failure");

    x = numtostr(0, llminnum);
    if (strcmp(x, llmin)) fail("numtostr() failure");
}


int main(void) {

    test_bufferoverflow();
    test_minmax();
    _exit(0);
}
