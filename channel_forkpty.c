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

#include "hasopenpty.h"
#ifdef HASOPENPTY
extern int openpty(int *, int *, char *, struct termios *, struct winsize *);
#endif

#include "haslogintty.h"
#ifdef HASLOGINTTY
extern int login_tty(int);
#endif

#include "e.h"
#include "coe.h"
#include "blocking.h"
#include "global.h"
#include "channel.h"


#ifndef HASLOGINTTY
static int login_tty_(int fd) {

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
#endif

#ifndef HASOPENPTY
static int openpty_(int *amaster, int *aslave) {

    int master = -1, slave = -1;
    char *slave_name;
    static const char *fn[] = { "/dev/ptmx", "/dev/ptc", 0 };
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
    if (aslave)  *aslave  = slave;
    return 0;
}
#endif

int channel_openpty(int *amaster, int *aslave) {

#ifdef HASOPENPTY
    if (openpty(amaster, aslave, 0, 0, 0) == -1) return 0;
#else
    if (openpty_(amaster, aslave) == -1) return 0;
#endif

   if (!ttyname(*aslave)) {
        close(*amaster);
        close(*aslave);
        return 0;
    }
    return 1;
}


/*
The 'channel_forkpty' function is used to create a new process
operating in a pseudoterminal. Function sets 3 integers in 'fd[3]':
fd[0] and fd[1] is pseudoterminal fd
fd[2] is always -1
*/

long long channel_forkpty(int fd[3], int master, int slave) {

    long long pid, r;
    char ch;
    int pi[2];

    if (!ttyname(slave)) return -1;
    if (pipe(pi) == -1) return -1;
    
    fd[0] = fd[1] = master;
    fd[2] = -1;

    pid = fork();
    switch (pid) {
        case -1:
            close(pi[0]);
            close(pi[1]);
            close(slave);
            close(master);
            return -1;
        case 0:
            close(master);
            close(pi[0]);
#ifdef HASLOGINTTY
            if (login_tty(slave) == -1) global_die(111);
#else
            if (login_tty_(slave) == -1) global_die(111);
#endif
            /* Trigger a read event on the other side of the pipe. */
            do {
                r = write(pi[1], "", 1);
            } while (r == -1 && errno == EINTR);
            close(pi[1]);

            return 0;
        default:
            close(pi[1]);
            coe_enable(master);
            blocking_disable(master);

            /*
            Wait until child calls login_tty(slave), so that we can safely
            close(slave). Fixes race condition between close(slave) in parent
            and login_tty(slave) in child process.
            */
            do {
                r = read(pi[0], &ch, sizeof ch);
            } while (r == -1 && errno == EINTR);
            close(pi[0]);

            close(slave);
            return pid;
    }
}
