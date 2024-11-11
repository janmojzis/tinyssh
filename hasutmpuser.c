/* Public domain. */
#include <time.h>
#include <sys/time.h>
#include <utmp.h>

int main(void) {

    struct utmp ut;
    if (sizeof ut.ut_user < 8) return 111;
    return 0;
}
