/* Public domain. */
#include <stdio.h>

extern void logwtmp(const char *, const char *, const char *);

static void fce(void) {

    logwtmp("", "", "");
}

int main(void) {

    printf("#define HASUTMPLOGWTMP 1\n");
    return 0;
}
