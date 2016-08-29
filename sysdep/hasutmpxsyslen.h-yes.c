/* Public domain. */
#include <utmpx.h>

int foo(struct utmpx *ut);
int foo(struct utmpx *ut) {
    return ut->ut_syslen;
}
