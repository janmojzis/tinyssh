/* Public domain. */
#include <stdio.h>
#include <sys/types.h>
#include <termios.h>
#include <util.h>

void fce(void) {

    int master;
    struct termios t;
    struct winsize w;
    char name[256];
    
    forkpty(&master, name, 0, 0);
}

int main(void) {

    printf("#ifndef _FORKPTY_H____\n");
    printf("#define _FORKPTY_H____\n\n");
    printf("#include <sys/types.h>\n");
    printf("#include <termios.h>\n");
    printf("#include <util.h>\n\n");
    printf("#endif\n");
    return 0;
}
