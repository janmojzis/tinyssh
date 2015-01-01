/* Public domain. */
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <utmp.h>

int main(void) {

    struct utmp ut;

    ut.ut_tv.tv_sec = 0;
    ut.ut_tv.tv_usec = 0;

    printf("#define HASUTMPTV 1\n");
    return 0;
}

