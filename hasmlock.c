/* Public domain. */
#include <sys/mman.h>

int main(void) {
    char x[16] = {0};
    mlock(x, sizeof x);
    munlock(x, sizeof x);
    return 0;
}
