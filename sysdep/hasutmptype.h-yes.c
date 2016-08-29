/* Public domain. */
#include <time.h>
#include <sys/time.h>
#include <utmp.h>

int foo(struct utmp *ut);
int foo(struct utmp *ut) {

    ut->ut_type = USER_PROCESS | DEAD_PROCESS;
    return ut->ut_type;
}
