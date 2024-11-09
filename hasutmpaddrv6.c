/* Public domain. */
#include <time.h>
#include <sys/time.h>
#include <utmp.h>

int main(void) {

    struct utmp ut;
    int x[sizeof(ut.ut_addr_v6) < 16 ? -1 : 1] = {0};
    return x[0];
}
