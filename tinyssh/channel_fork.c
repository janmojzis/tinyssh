/*
20140129
Jan Mojzis
Public domain.
*/

#include <unistd.h>
#include "blocking.h"
#include "open.h"
#include "channel.h"

/*
The 'channel_fork' function is used to create a new process.
Function creates 3 pipes from/to child:
fd[0] is pipe to childs standard input
fd[1] is pipe from childs standard output
fd[2] is pipe from childs error output
Function returns also childs PID.
*/
long long channel_fork(int fd[3]) {

    int pi[2], pa[3], ch[3];
    long long i, pid;

    for (i = 0; i < 3; ++i) pa[i] = ch[i] = fd[i] = -1;
    for (i = 0; i < 3; ++i) {
        if (open_pipe(pi) == -1) goto cleanup;
        pa[i] = pi[i ? 0 : 1];
        ch[i] = pi[i ? 1 : 0];
    }

    pid = fork();
    if (pid == -1) goto cleanup;
    if (pid == 0) {
        for (i = 0; i < 3; ++i) {
            close(pa[i]); close(i);
            blocking_enable(ch[i]);
            if (dup(ch[i]) != i) _exit(111);
        }
        return 0;
    }
    for (i = 0; i < 3; ++i) {
        close(ch[i]);
        fd[i] = pa[i];
    }

    return pid;

cleanup:
    for (i = 0; i < 3; ++i) { close(pa[i]); close(ch[i]); }
    return -1;
}
