/* Public domain. */
#include <utmpx.h>

int main(void) {

    struct utmpx ut = {0};
    return ut.ut_syslen;
}
