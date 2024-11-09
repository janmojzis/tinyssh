/* Public domain. */
#include <sys/mman.h>

int main(void) {
    char x[16];
    mlock(x, sizeof x);
    munlock(x, sizeof x);
    return 0;
}
