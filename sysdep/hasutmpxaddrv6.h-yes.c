/* Public domain. */
#include <time.h>
#include <sys/time.h>
#include <utmpx.h>

int main(void) {

    struct utmpx utx;
    if (sizeof utx.ut_addr_v6 < 16) return 111;
    return 0;
}
