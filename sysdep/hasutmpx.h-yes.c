/* Public domain. */
#include <stdio.h>
#include <utmpx.h>

void fce(void) {

    struct utmpx ut;

    ut.ut_type = DEAD_PROCESS;
    ut.ut_type = USER_PROCESS;
    ut.ut_type = 0;

    setutxent();
    pututxline(&ut);
    endutxent();
}

int main(void) {

    printf("#define HASUTMPX 1\n");
    return 0;
}

