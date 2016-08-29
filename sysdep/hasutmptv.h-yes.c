/* Public domain. */
#include <time.h>
#include <sys/time.h>
#include <utmp.h>

int foo(struct utmp *ut);
int foo(struct utmp *ut) {
    return ut->ut_tv.tv_sec + ut->ut_tv.tv_usec;
}
