/*
20140129
Jan Mojzis
Public domain.
*/

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "run.h"
#include "fail.h"
#include "newenv.h"

#define SCRIPT1 "\
err='newenvtest: failed: newenv_env does not set correcty variable'\n\
[ x\"$AA1\" = x ]   || ( echo $err; exit 111; ) \n\
[ x\"$AA3\" = x ]   || ( echo $err; exit 111; ) \n\
[ x\"$BBB\" = xok ] || ( echo $err; exit 111; ) \n\
[ x\"$CC1\" = xok ] || ( echo $err; exit 111; ) \n\
[ x\"$CC3\" = x ]   || ( echo $err; exit 111; ) \n\
[ x\"$DD1\" = xok ] || ( echo $err; exit 111; ) \n\
[ x\"$DD3\" = xok ] || ( echo $err; exit 111; ) \n\
[ x\"$EE1\" = xok ] || ( echo $err; exit 111; ) \n\
[ x\"$FF1\" = xok ] || ( echo $err; exit 111; ) \n\
[ x\"$YY1\" = xok ] || ( echo $err; exit 111; ) \n\
[ x\"$ZZ1\" = x ]   || ( echo $err; exit 111; ) \n\
[ x\"$ZZ3\" = x ]   || ( echo $err; exit 111; ) \n\
exit 0 \n\
"

void test1(void) {

    char *run[4];
    long long i;
    char buf[100];

    /* normal setenv */
    if (setenv("BBB", "ok", 1) == -1) fail("setenv failure");
    if (!newenv_copyenv("BBB")) fail("newenv_copyenv failure");

    /* normal env */
    if (!newenv_env("CC1", "ok")) fail("newenv_env failure");
    if (newenv_copyenv("CC3")) fail("newenv_copyenv failure");

    /* replaced env */
    if (!newenv_env("DD1", "failed")) fail("newenv_env failure");
    if (!newenv_env("DD1", "ok")) fail("newenv_env failure");
    if (setenv("DD3", "failed", 1) == -1) fail("setenv failure");
    if (!newenv_copyenv("DD3")) fail("newenv_copyenv failure");
    if (setenv("DD3", "ok", 1) == -1) fail("setenv failure");
    if (!newenv_copyenv("DD3")) fail("newenv_copyenv failure");

    /* setenv first */
    if (setenv("EE1", "failed", 1) == -1) fail("setenv failure");
    if (!newenv_env("EE1", "ok")) fail("newenv_env failure");

    /* setenv last */
    if (!newenv_env("FF1", "ok")) fail("newenv_env failure");
    if (setenv("FF1", "failed", 1) == -1) fail("setenv failure");

    /* replaced env -  ok */
    if (!newenv_env("YY1", "ok")) fail("newenv_env failure");
    for (i = 0; i < 200; ++i) {
        snprintf(buf, sizeof buf, "%lld", i);
        newenv_env(buf, buf);
    }
    if (!newenv_env("YY1", "ok")) fail("newenv_env failure");

    /* replaced env - overflow */
    for (i = 0; i < 200; ++i) {
        snprintf(buf, sizeof buf, "%lld", i);
        newenv_env(buf, buf);
    }
    if (newenv_env("ZZ1", "failed")) fail("newenv_env failure");
    if (setenv("ZZ3", "failed", 1) == -1) fail("setenv failure");
    if (newenv_copyenv("ZZ3")) fail("newenv_copyenv failure");


    run[0] = "/bin/sh";
    run[1] = "-ec";
    run[2] = SCRIPT1;
    run[3] = 0;
    newenv_exec(*run, run);
    fail("failure");
}


#define SCRIPT2 "\
err='newenvtest2: failed: environment should be empty'\n\
[ x\"$AAAA\" = x ] || ( echo $err; exit 111; ) \n\
exit 0 \n\
"


void test2(void) {

    char *run[4];

    /* setenv */
    if (setenv("AAAA", "failed", 1) == -1) fail("setenv failure");

    run[0] = "/bin/sh";
    run[1] = "-ec";
    run[2] = SCRIPT2;
    run[3] = 0;
    newenv_exec(*run, run);
}

void test3(void) {

    for (;;) {
        if (newenv_lowenv("XXX", "YYY")) break;
    }
    if (!newenv_env("XXX", "YYY")) fail("newenv_env failure");
}

void testdummy(void) {
    _exit(111);
}


int main(void) {
    run_mustpass(test1);
    run_mustpass(test2);
    test3();
    run_mustfail(testdummy);
    _exit(0);
}
