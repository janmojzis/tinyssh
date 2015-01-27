/* Public domain. */
#include <stdio.h>
#include <unistd.h>
/* #include <sys/stropts.h> */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <stdlib.h>

int main(void) {

    printf("#ifndef _FORKPTY_H____\n");
    printf("#define _FORKPTY_H____\n\n");
    printf("#define FORKPTYGENERIC 1\n\n");
    printf("#define _XOPEN_SOURCE 1\n\n");
    printf("#include <sys/types.h>\n");
    printf("#include <sys/ioctl.h>\n");
    printf("#include <termios.h>\n");
    printf("extern pid_t forkpty(int *, char *, struct termios *, struct winsize *);\n\n");
    printf("#endif\n");
    return 0;
}
