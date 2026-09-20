/*
20260920
Public domain.
*/

#include <fcntl.h>
#include <unistd.h>
#include "channel.h"
#include "global.h"

static int reader = -1;

long long channel_fork(int fd[3]) {
    int flags;
    int p[2];

    if (pipe(p) == -1) return -1;
    flags = fcntl(p[0], F_GETFL);
    if (flags == -1 || fcntl(p[0], F_SETFL, flags | O_NONBLOCK) == -1) {
        close(p[0]);
        close(p[1]);
        return -1;
    }
    reader = p[0];
    fd[0] = p[1];
    fd[1] = -1;
    fd[2] = -1;
    return 12345;
}

int main(void) {
    unsigned char ch;
    long long r;
    int ok = 1;

    global_init();
    channel.maxpacket = 32768;

    channel_puteof();
    if (!channel_exec("unused")) ok = 0;

    if (!channel.remoteeof) ok = 0;
    if (channel.pid != 12345) ok = 0;
    if (channel.fd0 != -1) ok = 0;
    if (channel.len0 != 0) ok = 0;
    if (channel_writeisready()) ok = 0;

    r = read(reader, &ch, 1);
    if (r != 0) ok = 0;

    if (channel.fd0 != -1) close(channel.fd0);
    if (reader != -1) close(reader);
    global_purge();
    return ok ? 0 : 111;
}
