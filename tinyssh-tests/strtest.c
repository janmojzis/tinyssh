/*
20130610
Jan Mojzis
Public domain.
*/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "fail.h"
#include "str.h"

void test_len(void) {

    if (str_len("") != 0) fail("str_len return's bad length");
    if (str_len("\0ahoj") != 0) fail("str_len return's bad length");
    if (str_len("a\0hoj") != 1) fail("str_len return's bad length");
    if (str_len("ahoj\0") != 4) fail("str_len return's bad length");
    if (str_len("ahoj") != 4) fail("str_len return's bad length");
}

void test_start(void) {

    if (!str_start("","")) fail("str_start failure");
    if (!str_start("x","")) fail("str_start failure");
    if (str_start("","x")) fail("str_start failure");
    if (!str_start("ahojx","ahoj")) fail("str_start failure");
    if (str_start("ahoj","ahojx")) fail("str_start failure");
}

void test_equal(void) {

#ifdef TODO
    /* equal */
    if (!str_equal("","")) fail("str_equal failure");
    if (!str_equal("\0","\0")) fail("str_equal failure");
    if (!str_equal("","\0")) fail("str_equal failure");
    if (!str_equal("\0","")) fail("str_equal failure");
    if (!str_equal("ahoj","ahoj")) fail("str_equal failure");
    if (!str_equal("ahoj","ahoj\0")) fail("str_equal failure");
    if (!str_equal("ahoj\0","ahoj")) fail("str_equal failure");

    /* not equal */
    if (str_equal("x","")) fail("str_equal failure");
    if (str_equal("","x")) fail("str_equal failure");
    if (str_equal("xx","x")) fail("str_equal failure");
    if (str_equal("x","xx")) fail("str_equal failure");
    if (str_equal("ahojx","ahoj")) fail("str_equal failure");
    if (str_equal("ahoj","ahojx")) fail("str_equal failure");
#endif
}

void test_equaln(void) {

    char buf[5];

    /* not equal */
    if (str_equaln(buf, 0, "a")) fail("str_equaln failure");
    if (str_equaln("ahoj", 4, "aho")) fail("str_equaln failure");
    if (str_equaln("ahoj", 4, "ahoj!")) fail("str_equaln failure");
    if (str_equaln("ahoj", 4, "ahox")) fail("str_equaln failure");

    /* equal */
    if (!str_equaln(buf, 0, "")) fail("str_equaln failure");
    if (strncmp(buf, "", 0)) fail("strncmp failure");
    if (!str_equaln("ahoj", 4, "ahoj")) fail("str_equaln failure");
    if (strncmp("ahoj", "ahoj", 4)) fail("strncmp failure");
}

void test_copyn(void) {

    char buf[5];

    /* bad */
    if (str_copyn(buf, 0, "")) fail("str_copyn failure");
    if (str_copyn(buf, 1, "a")) fail("str_copyn failure");
    if (str_copyn(buf, sizeof buf, "ahoj!")) fail("str_copyn failure");
    if (strcmp(buf, "ahoj")) fail("str_copyn failure");

    /* ok */
    if (!str_copyn(buf, sizeof buf, "ahoj")) fail("str_copyn failure");
    if (strcmp(buf, "ahoj")) fail("str_copyn failure");
    if (!str_copyn(buf, sizeof buf, "")) fail("str_copyn failure");
    if (strcmp(buf, "")) fail("str_copyn failure");
}

int main(void) {

    test_len();
    test_start();
    test_equal();
    test_equaln();
    test_copyn();
    _exit(0);
}
