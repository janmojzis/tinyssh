/* Public domain. */
#include <sys/mman.h>

int foo(char *x, size_t xlen);
int foo(char *x, size_t xlen) {

    if (mlock(x, xlen) == -1) return 1;
    if (munlock(x, xlen) == -1) return 1;
    return 0;
}
