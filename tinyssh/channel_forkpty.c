/*
20150212
Jan Mojzis
Public domain.
*/

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

#include "coe.h"
#include "blocking.h"
#include "global.h"
#include "channel.h"


static int _login_tty(int fd) {

    char *name;

    setsid();

#ifdef TIOCSCTTY
    if (ioctl(fd, TIOCSCTTY, (char *)0) == -1) return -1;
#endif

    name = ttyname(fd);
    if (!name) return -1;

#ifndef TIOCSCTTY
    if (fd != 0) close(0);
    if (fd != 1) close(1);
    if (fd != 2) close(2);
    close(open(name, O_RDWR));
#endif

    if (dup2(fd, 0) == -1) return -1;
    if (dup2(fd, 1) == -1) return -1;
    if (dup2(fd, 2) == -1) return -1;
    if (fd > 2) close(fd);
    return 0;
}


int channel_openpty(int *amaster, int *aslave) {

    int master = -1, slave = -1;
    char *slave_name;
    static const char *fn[] = { "/dev/ptmx", "/dev/ptc", 0 };
    long long i;

    for (i = 0; fn[i]; ++i) {
        master = open(fn[i], O_RDWR | O_NOCTTY);
        if (master != -1) break;
    }
    if (master == -1) return 0;

    if (grantpt(master) == -1) { close(master); return 0; }
    if (unlockpt(master) == -1) { close(master); return 0; }
    slave_name = ptsname(master);
    if (!slave_name) { close(master); return 0; }
    slave = open(slave_name, O_RDWR | O_NOCTTY);
    if (slave == -1) { close(master); return 0; }
#if defined(sun) || defined(__hpux)
    ioctl(slave, I_PUSH, "ptem");
    ioctl(slave, I_PUSH, "ldterm");
#endif
#if defined(sun)
    ioctl(slave, I_PUSH, "ttcompat");
#endif

    if (!ttyname(slave)) {
        close(master);
        close(slave);
        return 0;
    }

    if (amaster) *amaster = master;
    if (amaster) *aslave  = slave;
    return 1;
}

/*
The 'channel_forkpty' function is used to create a new process
operating in a pseudoterminal. Function sets 3 integers in 'fd[3]':
fd[0] and fd[1] is pseudoterminal fd
fd[2] is always -1
*/

long long channel_forkpty(int fd[3], int master, int slave) {

    long long pid;
    
    fd[0] = fd[1] = master;
    fd[2] = -1;

    pid = fork();
    switch (pid) {
        case -1:
            close(slave);
            close(master);
            return -1;
        case 0:
            close(master);
            if (_login_tty(slave) == -1) global_die(111);
            return 0;
        default:
            coe_enable(master);
            blocking_disable(master);
            close(slave);
            return pid;
    }
}
