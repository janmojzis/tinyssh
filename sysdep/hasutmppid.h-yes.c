/* Public domain. */
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <utmp.h>

int main(void) {

    struct utmp ut;

    ut.ut_pid = 0;

    printf("#define HASUTMPPID 1\n");
    return 0;
}

