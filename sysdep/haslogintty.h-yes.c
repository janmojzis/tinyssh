/* Public domain. */
#include <stdio.h>

extern int login_tty(int);

static void fce(void) {
    login_tty(0);
}

int main(void) {

    printf("#define HASLOGINTTY 1\n");
    return 0;
}
