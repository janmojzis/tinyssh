/*
20140320
Jan Mojzis
Public domain.
*/

#include <unistd.h>
#include "run.h"
#include "fail.h"
#include "dropuidgid.h"

static void droproot(void) {

    if (geteuid() == 0) {
        if (!dropuidgid(0, 1230, 1230)) _exit(111);
    }
    if (geteuid() == 0) _exit(111);
}


static void test1(void) {
    droproot();
    if (dropuidgid(0, 1231, getegid())) _exit(111);
    _exit(0);
}

static void test2(void) {
    droproot();
    if (!dropuidgid(0, geteuid(), getegid())) _exit(111);
    if (!dropuidgid(0, geteuid(), getegid())) _exit(111);
    _exit(0);
}

static void test3(void) {
    droproot();
    if (!dropuidgid(0, 0, 0)) _exit(111);
    _exit(0);
}


int main(void) {

    run_mustpass(test1);
    run_mustpass(test2);
    run_mustfail(test3);
    _exit(0);
}
