/*
20140227
Jan Mojzis
Public domain.
*/

/*
The 'logger' library is used to execute logger program.
For security reasons is never executed under root privileges. 
*/

#include <sys/types.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>
#include "e.h"
#include "writeall.h"
#include "dropuidgid.h"

static int sandbox(void) {

    long long id;
    struct rlimit r;

    if (geteuid() == 0) {
        /* NOTE: if getpid() fails, UID = 141499999,
        not perfect, but not dangerous */
        id = 141500000 + getpid(); 
        if (!dropuidgid(id, id)) return -1;
    }

    /* prohibit fork */
#ifdef RLIMIT_NPROC
    if (getrlimit(RLIMIT_NPROC, &r) == -1) return -1;
    r.rlim_cur = 0;
    r.rlim_max = 0;
    if (setrlimit(RLIMIT_NPROC, &r) == -1) return -1;
#endif
    return 0;
}

int syslogger(void) {

    char *logger[2] = { "loggger", 0 };
    pid_t pid;
    int tochild[2] = { -1, -1 };

    if (pipe(tochild) == -1) return -1;
    pid = fork();
    if (pid == -1) return -1;

    if (pid == 0) {
        if (sandbox() == -1) _exit(111);
        close(tochild[1]);
        if (dup2(tochild[0], 0) == -1) _exit(111);
        execvp(*logger, logger);
        _exit(111);
    }
    close(tochild[0]);
    if (dup2(tochild[1], 2) == -1) return -1;
    return 0;
}
