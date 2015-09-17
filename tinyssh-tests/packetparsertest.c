/*
20140303
Jan Mojzis
Public domain.
*/

#include <unistd.h>
#include "run.h"
#include "fail.h"
#include "byte.h"
#include "packetparser.h"

/* 
checks packetparser_uint8(), packetparser_uint32() function
and packetparser_end called more than once
 */
static void test1(void) {

    unsigned char buf[4];
    crypto_uint8 ch;
    crypto_uint32 u;
    long long i, pos = 0;

    byte_copy(buf, 4, "abcd");

    /* uint8 */
    pos = 0;
    for (i = 0; i < sizeof buf; ++i) {
        pos = packetparser_uint8(buf, sizeof buf, pos, &ch);
        if (ch != buf[i]) fail("failure");
    }
    if (pos != sizeof buf) fail("failure");

    /* uint32 */
    pos = 0;
    pos = packetparser_uint8(buf, sizeof buf, 0, &ch);
    pos = packetparser_uint32(buf, sizeof buf, 0, &u);
    if (pos != sizeof buf) fail("failure");

    pos = packetparser_end(buf, sizeof buf, pos);
    pos = packetparser_end(buf, sizeof buf, pos);
    pos = packetparser_end(buf, sizeof buf, pos);
}


/* checks if packetparser_skip() skips 0 bytes */
static void test2(void) {

    unsigned char buf[5];
    crypto_uint8 ch;
    crypto_uint32 len;
    long long i, pos = 0;

    for (i = 0; i < sizeof buf; ++i) buf[i] = 0;

    pos = packetparser_uint32(buf, sizeof buf, pos, &len);
    if (len != 0) fail("failure");
    pos = packetparser_skip(buf, sizeof buf, pos, len);
    pos = packetparser_uint8(buf, sizeof buf, pos, &ch);
    if (ch) fail("failure");
    if (pos != sizeof buf) fail("failure");

    pos = packetparser_end(buf, sizeof buf, pos);
    pos = packetparser_end(buf, sizeof buf, pos);
    pos = packetparser_end(buf, sizeof buf, pos);
}

/* checks if packetparser_copy() accepts 0 bytes */
static void test4(void) {

    unsigned char buf[5];
    unsigned char buf2[6];
    crypto_uint8 ch;
    crypto_uint32 len;
    long long i, pos = 0;

    for (i = 0; i < sizeof buf; ++i) buf[i] = 0;

    pos = packetparser_uint32(buf, sizeof buf, pos, &len);
    if (len != 0) fail("failure");
    pos = packetparser_copy(buf, sizeof buf, pos, buf2, len);
    pos = packetparser_uint8(buf, sizeof buf, pos, &ch);
    if (ch) fail("failure");
    if (pos != sizeof buf) fail("failure");

    pos = packetparser_end(buf, sizeof buf, pos);
    pos = packetparser_end(buf, sizeof buf, pos);
    pos = packetparser_end(buf, sizeof buf, pos);
}


/* packetparser_skip() overflow */
static void _test3a(void) {

    unsigned char buf[5];
    long long pos = 0;

    pos = packetparser_skip(buf, sizeof buf, pos, 6);
}


static void _test3aa(void) {

    unsigned char buf[5];
    long long len, pos;
    len = pos = 9223372036854775807LL;

    pos = packetparser_skip(buf, len, pos, 1);
}

/* packetparser_uint32() overflow */
static void _test3b(void) {

    unsigned char buf[3];
    crypto_uint32 u;
    long long pos = 0;

    pos = packetparser_uint32(buf, sizeof buf, pos, &u);
}

static void _test3bb(void) {

    unsigned char buf[4];
    crypto_uint32 u;
    long long len, pos;
    len = pos = 9223372036854775807LL;
    
    pos = packetparser_uint32(buf, len, pos, &u);
}



/* packetparser_uint8() overflow */
static void _test3c(void) {

    unsigned char buf[3];
    crypto_uint8 ch;
    long long pos = 0;

    pos = packetparser_uint8(buf, sizeof buf, pos, &ch);
    pos = packetparser_uint8(buf, sizeof buf, pos, &ch);
    pos = packetparser_uint8(buf, sizeof buf, pos, &ch);
    pos = packetparser_uint8(buf, sizeof buf, pos, &ch);
}

static void _test3cc(void) {

    unsigned char buf[3];
    crypto_uint8 ch;
    long long len, pos;
    len = pos = 9223372036854775807LL;

    pos = packetparser_uint8(buf, len, pos, &ch);
}

/* packetparser_copy() overflow */
static void _test3d(void) {

    unsigned char buf1[10];
    unsigned char buf2[11];
    long long pos = 0;

    pos = packetparser_copy(buf1, sizeof buf1, pos, buf2, sizeof buf2);
}

static void _test3dd(void) {

    unsigned char buf1[10];
    unsigned char buf2[10];
    long long len, pos;
    len = pos = 9223372036854775807LL;

    pos = packetparser_copy(buf1, len, pos, buf2, sizeof buf2);
}


/* packetparser_end() failure */
static void _test3e(void) {

    unsigned char buf[1];

    packetparser_end(buf, 0, 1);
}

static void _test3ee(void) {

    unsigned char buf[1];
    long long pos = 9223372036854775807LL;
    
    packetparser_end(buf, pos, pos);
}


static void test3(void) {
    run_mustfail(_test3a);
    run_mustfail(_test3aa);
    run_mustfail(_test3b);
    run_mustfail(_test3bb);
    run_mustfail(_test3c);
    run_mustfail(_test3cc);
    run_mustfail(_test3d);
    run_mustfail(_test3dd);
    run_mustfail(_test3e);
    run_mustfail(_test3ee);
}

/* dummy test */
static void testdummy(void) {
    _exit(0);
}


int main(void) {

    test1();
    test2();
    test3();
    test4();
    run_mustpass(testdummy);
    _exit(0);
}
