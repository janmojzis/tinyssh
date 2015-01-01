/* Public domain. */
#include <stdio.h>
#include <utmpx.h>

int main(void) {

    struct utmpx ut = {0};

    ut.ut_syslen = 0;

    printf("#define HASUTMPXSYSLEN 1\n");
    return 0;
}
