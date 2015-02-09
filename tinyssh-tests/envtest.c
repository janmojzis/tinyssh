/*
20130606
Jan Mojzis
Public domain.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "fail.h"
#include "env.h"

static void test_zero(void) {

    if (env_get(0)) fail("env_get accepts zero input");
}

static void doit(const char *d0, const char *d) {

    char *s;

    s = env_get(d0);

    if (!s) {
        if (d) fail("env_get failure");
        return;
    }

    if (!s) fail("env_get failure");
    if (strcmp(s, d)) fail("env_get failure");
    return;
}

static void test_setunset(void) {

    setenv("x","x",1);
    doit("x", "x");

    setenv("x","y",1);
    doit("x", "y");

    unsetenv("x");
    doit("x", 0);
}


static void test_env(void) {

    const char *env[10];

    env[0] = "a=1";
    env[1] = "a=2";
    env[2] = "b=b=1";
    env[3] = "c=";
    env[4] = "d";
    env[5] = "=null";
    env[6] = "e==";
    env[7] = 0;

    environ = (char **)env;

    doit("a", "1");
    doit("b", "b=1");
    doit("b=b", "1");
    doit("c", "");
    doit("d", 0);
    doit("e", "=");
    doit("", "null");
}

int main(void) {

    test_zero();
    test_setunset();
    test_env();
    _exit(0);
}
