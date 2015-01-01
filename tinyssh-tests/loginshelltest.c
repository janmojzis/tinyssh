/*
20140429
Jan Mojzis
Public domain.
*/

#include "str.h"
#include "byte.h"
#include "fail.h"
#include "loginshell.h"

static struct vector {
    const char *in;
    const char *out;
} vectors[] = {
    { "",  "-" },
    { "/", "-" },
    { "//", "-" },
    { "///", "-" },
    { "////", "-" },
    { "/////", "-" },
    { "/a/b/c/d/", "-" },
    { "/a///d/", "-" },
    { "//b/c//", "-" },
    { "x/", "-" },
    { "x/x/xx/xxx/", "-" },
    { "sh", "-sh" },
    { "/bin/bash", "-bash" },
    { "///////dash", "-dash" },
    { "///////-ksh", "--ksh" },
    { 0, 0 }
};

static void testok(void) {

    long long i, len;
    char buf[10];

    for (i = 0; vectors[i].in; ++i) {
        if (!loginshell(buf, sizeof buf, vectors[i].in)) fail("loginshell failure");
        len = str_len(vectors[i].out);
        if (str_len(buf) != len) fail("loginshell failure");
        if (!byte_isequal(buf, len, vectors[i].out)) fail("loginshell failure");
    }
}

static void testbad(void) {

    char buf[10];

    if (loginshell(buf, -1, "")) fail("loginshell failure");
    if (loginshell(buf, 0, "")) fail("loginshell failure");
    if (loginshell(buf, 1, "")) fail("loginshell failure");
    if (loginshell(buf, 1, "//")) fail("loginshell failure");
    if (loginshell(buf, 3, "/bin/sh")) fail("loginshell failure");
}


int main(void) {

    testok();
    testbad();
    _exit(0);
}
