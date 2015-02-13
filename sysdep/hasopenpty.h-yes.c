/* Public domain. */
#include <stdio.h>
#include <sys/types.h>
#include <termios.h>

extern int openpty(int *, int *, char *, struct termios *, struct winsize *);

void fce(void) {

    int master, slave;
    openpty(&master, &slave, 0, 0, 0);
}

int main(void) {

    printf("#define HASOPENPTY 1\n");
    return 0;
}
