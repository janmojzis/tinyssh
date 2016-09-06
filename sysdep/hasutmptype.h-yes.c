/* Public domain. */
#include <time.h>
#include <sys/time.h>
#include <utmp.h>

int main(void) {

    struct utmp ut;
    ut.ut_type = USER_PROCESS | DEAD_PROCESS;
    return 0;
}
