/* Public domain. */
#include <time.h>
#include <sys/time.h>
#include <utmp.h>

struct utmp *foo(struct utmp *ut);
struct utmp *foo(struct utmp *ut) {
    return ut;
}
