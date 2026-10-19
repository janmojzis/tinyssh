/*
20150212
20241209 - reformated using clang-format
Jan Mojzis
Public domain.
*/

#include <unistd.h>
#include "e.h"
#include "coe.h"
#include "p.h"
#include "blocking.h"
#include "global.h"
#include "channel.h"

int channel_openpty(int *amaster, int *aslave) {

    if (p_openpty(amaster, aslave) == -1) return 0;

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
            if (p_login_tty(slave) == -1) global_die(111);
            /* Trigger a read event on the other side of the pipe. */
            do { r = write(pi[1], "", 1); } while (r == -1 && errno == EINTR);
            close(pi[1]);
            p_closefrom(3);
            errno = 0;

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
