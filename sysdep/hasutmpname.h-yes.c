/* Public domain. */
#include <time.h>
#include <sys/time.h>
#include <utmp.h>

char *foo(struct utmp *ut);
char *foo(struct utmp *ut) {
    return ut->ut_name;
}

