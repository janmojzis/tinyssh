/* Public domain. */
#include <time.h>
#include <sys/time.h>
#include <utmpx.h>

int main(void) {

    struct utmpx utx;
    int x[sizeof(utx.ut_addr_v6) < 16 ? -1 : 1] = {0};
    return x[0];
}
