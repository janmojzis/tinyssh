/*
20140219
Jan Mojzis
Public domain.

from Linux manpage:
The  forkpty()  function  combines openpty(), fork(2), and login_tty()
to create a new process operating in a pseudoterminal.  The file descriptor
of the master side of the pseudoterminal is returned in amaster, and the filename
of the slave in name if it is not NULL.  The termp and winp arguments, if not NULL,
will determine the  terminal attributes and window size of the slave side of the pseudoterminal
*/

#include "forkpty.h"

#ifdef FORKPTYGENERIC
#include <unistd.h>
#if defined(sun) || defined(__hpux)
#include <sys/stropts.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/ioctl.h> 
extern char *ptsname(int);
extern int grantpt(int);
extern int unlockpt(int);

const char *fn[] = { "/dev/ptmx", "/dev/ptc", 0 };

pid_t forkpty(int *amaster, char *name, struct termios *termp, struct winsize *winp) {

    int master = -1, slave;
    char *slave_name; 
    pid_t pid; 
    long long i;

    for (i = 0; fn[i]; ++i) {
        master = open(fn[i], O_RDWR | O_NOCTTY);
        if (master != -1) break;
    }
    if (master == -1) return -1;

    if (grantpt(master) == -1) { close(master); return -1; }
    if (unlockpt(master) == -1) { close(master); return -1; }
    slave_name = ptsname(master); 
    if (!slave_name) { close(master); return -1; }
    slave = open(slave_name, O_RDWR | O_NOCTTY);
    if (slave == -1) { close(master); return -1; }
#if defined(sun) || defined(__hpux)
    ioctl(slave, I_PUSH, "ptem");
    ioctl(slave, I_PUSH, "ldterm");
#endif
#if defined(sun)
    ioctl(slave, I_PUSH, "ttcompat");
#endif

    if (amaster) *amaster = master; 

    /* for compatibility with forkpty - in TinySSH: name = termp = winp = 0 */
    if (name) { for (i = 0; slave_name[i]; ++i) name[i] = slave_name[i]; name[i] = 0; }
    if (termp) tcsetattr(slave, TCSAFLUSH, termp);
    if (winp) ioctl(slave, TIOCSWINSZ, winp);

    pid = fork();
    switch (pid) {
        case -1:
            return -1;
        case 0:
            close(master);
            setsid();
#ifdef TIOCSCTTY
            ioctl(slave, TIOCSCTTY, (char *)0);
#endif
            if (dup2(slave, 0) == -1) _exit(111);
            if (dup2(slave, 1) == -1) _exit(111);
            if (dup2(slave, 2) == -1) _exit(111);
            return 0;
        default:
            close(slave);
            return pid;
    }
}
#endif
