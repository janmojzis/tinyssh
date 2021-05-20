/* Public domain. */
#include <time.h>
#include <sys/time.h>
#include <utmp.h>

static void foo(void) {
    struct utmp ut = {0};
    login(&ut);
    logout(ut.ut_line);
}

int main(void) {
    return 0;
}
