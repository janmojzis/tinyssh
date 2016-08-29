/* Public domain. */
#include <time.h>
#include <sys/time.h>
#include <utmp.h>

extern void login(const struct utmp *);
extern int logout(const char *);

void foo(const struct utmp *ut);
void foo(const struct utmp *ut) {

    login(ut);
    logout(ut->ut_line);
}
