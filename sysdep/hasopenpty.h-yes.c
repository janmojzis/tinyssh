/* Public domain. */
#include <sys/types.h>
#include <termios.h>

extern int openpty(int *, int *, char *, struct termios *, struct winsize *);

void foo(int *amaster, int *aslave, char *name, struct termios *termp, struct winsize *winp);
void foo(int *amaster, int *aslave, char *name, struct termios *termp, struct winsize *winp) {
    openpty(amaster, aslave, name, termp, winp);
}
