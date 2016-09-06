/* Public domain. */
#include <utmpx.h>


static void foo(void) {

    struct utmpx ut;

    ut.ut_type = DEAD_PROCESS;
    ut.ut_type = USER_PROCESS;
    ut.ut_type = 0;

    setutxent();
    pututxline(&ut);
    endutxent();
}

int main(void) {
    return 0;
}
