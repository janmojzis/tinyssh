/*
20140227
Jan Mojzis
Public domain.
*/

/*
The 'subprocess_syslog' function reads log messages and feeds them to syslog.
For security reasons is never executed under root privileges. 
*/

#include <sys/types.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <syslog.h>
#include "e.h"
#include "dropuidgid.h"
#include "global.h"
#include "blocking.h"
#include "getln.h"
#include "subprocess.h"

int subprocess_syslog(void) {

    pid_t pid;
    int tochild[2] = { -1, -1 };

    if (open_pipe(tochild) == -1) return -1;
    pid = fork();
    if (pid == -1) return -1;

    if (pid == 0) {
        pid_t ppid;
        long long i;
        struct rlimit r;
        #define buf global_bspace1

        close(tochild[1]);
        close(0);
        if (dup(tochild[0]) != 0) global_die(111);
        close(1);

        /* drop root */
        if (geteuid() == 0) {
            /* NOTE: if getpid() fails, UID = 141499999,
            not perfect, but not dangerous */
            i = 141500000 + getpid();
            if (!dropuidgid(i, i)) global_die(111);
        }

        /* prohibit fork */
#ifdef RLIMIT_NPROC
        if (getrlimit(RLIMIT_NPROC, &r) == -1) global_die(111);
        r.rlim_cur = 0;
        r.rlim_max = 0;
        if (setrlimit(RLIMIT_NPROC, &r) == -1) global_die(111);
#endif

        openlog("tinysshd", 0, LOG_DAEMON);

        ppid = getppid();
        while (ppid == getppid()) {
            i = getln(0, buf, sizeof buf);
            if (i == 0) break; 
            if (i == -1) if (errno != ENOMEM) break;
            syslog(LOG_INFO, "%s", buf);
        }
        closelog();
        global_die(0);
    }
    close(tochild[0]);
    blocking_enable(tochild[1]);
    if (dup2(tochild[1], 2) == -1) return -1;
    return 0;
}
