#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include "haslogintty.h"
#include "p.h"

#ifdef HASLOGINTTY
extern int login_tty(int);
#endif

/* Attach fd as the controlling terminal and standard input/output/error. */
int p_login_tty(int fd) {

#ifdef HASLOGINTTY
    return login_tty(fd);
#else
    char *name;

    setsid();

#ifdef TIOCSCTTY
    if (ioctl(fd, TIOCSCTTY, (char *) 0) == -1) return -1;
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
#endif
}
