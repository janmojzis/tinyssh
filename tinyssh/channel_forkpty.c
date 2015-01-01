/*
20140129
Jan Mojzis
Public domain.
*/

#include "coe.h"
#include "blocking.h"
#include "forkpty.h"
#include "channel.h"

/*
The 'channel_forkpty' function is used to create a new process
operating in a pseudoterminal. Function sets 3 integers in 'fd[3]':
fd[0] and fd[1] is pseudoterminal fd
fd[2] is always -1
*/
long long channel_forkpty(int fd[3]) {

    int master;
    long long pid;

    fd[0] = fd[1] = fd[2] = -1;

    pid = forkpty(&master, 0, 0, 0);
    if (pid == -1) return -1;

    if (pid > 0) {
        coe_enable(master);
        blocking_disable(master);
    }

    fd[0] = fd[1] = master;
    return pid;
}
