/* Public domain. */
#include <stdio.h>

extern logwtmp(const char *, const char *, const char *);

void fce(void) {

    logwtmp("", "", "");
}

int main(void) {

    printf("#define HASUTMPLOGWTMP 1\n");
    return 0;
}
