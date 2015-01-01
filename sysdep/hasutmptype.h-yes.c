/* Public domain. */
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <utmp.h>

int main(void) {

    struct utmp ut;

    ut.ut_type = 0;
    ut.ut_type = USER_PROCESS;
    ut.ut_type = DEAD_PROCESS;

    printf("#define HASUTMPTYPE 1\n");
    return 0;
}

