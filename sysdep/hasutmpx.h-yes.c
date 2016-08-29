/* Public domain. */
#include <utmpx.h>

void foo(struct utmpx *ut);
void foo(struct utmpx *ut) {

    ut->ut_type = DEAD_PROCESS;
    ut->ut_type = USER_PROCESS;
    ut->ut_type = 0;

    setutxent();
    pututxline(ut);
    endutxent();
}
