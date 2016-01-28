/* Public domain. */
#include <stdio.h>
#include <utmpx.h>

static void fce(void) {
    struct utmpx ut = {0};
    updwtmpx("/nonexistent", &ut);
}

int main(void) {

    printf("#define HASUTMPXUPDWTMPX 1\n");
    return 0;
}
