/* Public domain. */
#include <utmpx.h>

static void foo(void) {
    struct utmpx ut = {0};
    updwtmpx("/nonexistent", &ut);
}

int main(int argc, char **argv) {
    if (!argc)
        foo();
    return 0;
}
