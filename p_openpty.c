#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/ioctl.h>
#include "hasopenpty.h"
#include "p.h"

#ifdef HASOPENPTY
extern int openpty(int *, int *, char *, struct termios *, struct winsize *);
#else
#if defined(sun) || defined(__hpux)
#include <sys/stropts.h>
#endif
extern char *ptsname(int);
extern int grantpt(int);
extern int unlockpt(int);
#endif

/* Open a pseudoterminal pair; return 0 on success or -1 on failure. */
int p_openpty(int *amaster, int *aslave) {

#ifdef HASOPENPTY
    return openpty(amaster, aslave, 0, 0, 0);
#else
    int master = -1, slave = -1;
    char *slave_name;
    static const char *fn[] = {"/dev/ptmx", "/dev/ptc", 0};
    long long i;

    for (i = 0; fn[i]; ++i) {
        master = open(fn[i], O_RDWR | O_NOCTTY);
        if (master != -1) break;
    }
    if (master == -1) return -1;

    if (grantpt(master) == -1) {
        close(master);
        return -1;
    }
    if (unlockpt(master) == -1) {
        close(master);
        return -1;
    }
    slave_name = ptsname(master);
    if (!slave_name) {
        close(master);
        return -1;
    }
    slave = open(slave_name, O_RDWR | O_NOCTTY);
    if (slave == -1) {
        close(master);
        return -1;
    }
#if defined(sun) || defined(__hpux)
    ioctl(slave, I_PUSH, "ptem");
    ioctl(slave, I_PUSH, "ldterm");
#endif
#if defined(sun)
    ioctl(slave, I_PUSH, "ttcompat");
#endif

    if (amaster) *amaster = master;
    if (aslave) *aslave = slave;
    return 0;
#endif
}
