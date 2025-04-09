/*
20140204
Jan Mojzis
Public domain.
*/

#include <unistd.h>
#include <stdio.h>
#include "fail.h"
#include "str.h"
#include "byte.h"
#include "stringparser.h"

static struct vectors {
    const char *buf;
} testvectors[] = {
    { "aaa,bb,c" },
    { ",aaa,bb,c" },
    { "aaa,bb,c," },
    { "aaa,,,,bb,,,,c" },
    { ",,,aaa,,,,bb,,,,c" },
    { ",,,aaa,,,,bb,,,,c,,," },
    { "aaa,bb,,c," },
    { 0 }
};

static void stringstringparser_test1(void) {

    unsigned char *buf;
    long long i, pos = 0;
    const unsigned char *x;
    long long xlen, len;

    for (i = 0; testvectors[i].buf; ++i) {
        buf = (unsigned char *)testvectors[i].buf;
        len = str_len(testvectors[i].buf);
        pos = 0;

        pos = stringparser(buf, len, pos, &x, &xlen); if (!pos) fail("parsing error");
        if (xlen != 3) fail("parsing error");
        if (!byte_isequal(x, xlen, "aaa")) fail("parsing error");

        pos = stringparser(buf, len, pos, &x, &xlen); if (!pos) fail("parsing error");
        if (xlen != 2) fail("parsing error");
        if (!byte_isequal(x, xlen, "bb")) fail("parsing error");

        pos = stringparser(buf, len, pos, &x, &xlen); if (!pos) fail("parsing error");
        if (xlen != 1) fail("parsing error");
        if (!byte_isequal(x, xlen, "c")) fail("parsing error");

        if (stringparser(buf, len, pos, &x, &xlen)) fail("overflow");
    }
}

static void stringstringparser_test2(void) {

    unsigned char *buf;
    long long pos = 0;
    const unsigned char *x;
    long long xlen, len;

    buf = (unsigned char *)"";
    len = str_len((char *)buf);
    pos = 0;
    if (stringparser(buf, len, pos, &x, &xlen)) fail("overflow");

    buf = (unsigned char *)",";
    len = str_len((char *)buf);
    pos = 0;
    if (stringparser(buf, len, pos, &x, &xlen)) fail("overflow");

    buf = (unsigned char *)",,,,,,,,";
    len = str_len((char *)buf);
    pos = 0;
    if (stringparser(buf, len, pos, &x, &xlen)) fail("overflow");

}

int main(void) {

    stringstringparser_test1();
    stringstringparser_test2();
    _exit(0);
}
