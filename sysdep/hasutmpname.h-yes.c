/* Public domain. */
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <utmp.h>

int main(void) {

    struct utmp ut;

    if (sizeof ut.ut_name < 8) return 0;

    printf("#define HASUTMPNAME 1\n");
    return 0;
}

