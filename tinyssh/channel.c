/*
20140124
Jan Mojzis
Public domain.

The 'channel' library is used to handle data from/to SSH channel (rfc4254).
*/

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <paths.h>
extern char *ptsname(int);
#include "byte.h"
#include "bug.h"
#include "newenv.h"
#include "e.h"
#include "purge.h"
#include "connectioninfo.h"
#include "iptostr.h"
#include "porttostr.h"
#include "buf.h"
#include "str.h"
#include "logsys.h"
#include "loginshell.h"
#include "trymlock.h"
#include "limit.h"
#include "channel.h"

/*
1. channel not open:    maxpacket == 0, pid == 0
2. channel open:        maxpacket != 0, pid == 0
3. child executed:      maxpacket != 0, pid > 0, fd[01] != -1
4. child eof:           maxpacket != 0, pid > 0, fd[12] == -1
5. child died:          maxpacket != 0, pid == -1
*/

struct channel channel = {0};

/*
The 'channel_open' function opens the channel.
It sets 'localwindow' and maxpacket, values obtained from 
from SSH_MSG_CHANNEL_OPEN message.
Function also obtaines connection information and sets
environment variables PATH, SSH_CONNECTION and MAIL.
*/
int channel_open(const char *user, crypto_uint32 id, crypto_uint32 remotewindow, crypto_uint32 maxpacket, crypto_uint32 *localwindow) {

    struct buf b = { channel.buf0, 0, CHANNEL_BUFSIZE };

    if (!localwindow) bug_inval();
    if (!maxpacket) bug_inval();
    if (!remotewindow) bug_inval();
    if (channel.maxpacket != 0) return 0;
    if (channel.pid != 0) return 0;

    /* copy user-name */
    if (!str_copyn(channel.user, sizeof channel.user, user)) bug_nomem();

    /* set id, maxpacket, remotewindow, localwindow */
    channel.id = id;
    channel.maxpacket    = maxpacket;
    channel.remotewindow = remotewindow;
    channel.localwindow  = *localwindow = CHANNEL_BUFSIZE;

    /* copy PATH */
    if (!newenv_copyenv("PATH")) if (!newenv_env("PATH", "/bin:/usr/bin")) return 0;

    /* create env. SSH_CONNECTION */
    connectioninfo(channel.localip, channel.localport, channel.remoteip, channel.remoteport);
    buf_purge(&b);
    buf_puts(&b, channel.remoteip);
    buf_puts(&b, " ");
    buf_puts(&b, channel.remoteport);
    buf_puts(&b, " ");
    buf_puts(&b, channel.localip);
    buf_puts(&b, " ");
    buf_puts(&b, channel.localport);
    buf_putnum8(&b, 0);
    if (!newenv_env("SSH_CONNECTION", (char *)b.buf)) return 0;

    /* create env. MAIL */
#ifdef _PATH_MAILDIR
    buf_purge(&b);
    buf_puts(&b, _PATH_MAILDIR);
    buf_puts(&b, "/");
    buf_puts(&b, user);
    buf_putnum8(&b, 0);
    if (!newenv_env("MAIL", (char *)b.buf)) return 0;
#endif

    purge(channel.buf0, sizeof channel.buf0);
    return 1;
}

/*
The 'channel_openterminal' function opens terminal,
sets environment variable TERM and initial terminal windowsize.
*/
int channel_openterminal(const char *name, crypto_uint32 a, crypto_uint32 b, crypto_uint32 x, crypto_uint32 y) {

    if (channel.maxpacket == 0) bug_proto();
    if (channel.pid != 0) bug_proto();
    if (channel.flagterminal == 1) bug_proto();

    if (!channel_openpty(&channel.master, &channel.slave)) return 0;
    if (!newenv_env("TERM", name)) return 0;

    channel.flagterminal = 1;
    channel.a = a;
    channel.b = b;
    channel.x = x;
    channel.y = y;
    return 1;
}

/*
The 'channel_ptyresize' function sets new
terminal windowsize.
*/
void channel_ptyresize(crypto_uint32 a, crypto_uint32 b, crypto_uint32 x, crypto_uint32 y) {

    struct winsize w;

    if (channel.maxpacket == 0) bug_proto();
    if (channel.pid <= 0) bug_proto();
    if (!a && !b && !x && !y) return;

    w.ws_col = a;
    w.ws_row = b;
    w.ws_xpixel = x;
    w.ws_ypixel = y;
    ioctl(channel.fd0, TIOCSWINSZ, &w);
}

/*
The 'channel_env' adds new environment variable
sent from client.
*/
int channel_env(const char *x, const char *y) {

    if (channel.maxpacket == 0) bug_proto();
    if (channel.pid != 0) bug_proto();
    if (!x || !y) bug_inval();
    return newenv_lowenv(x, y);
}

/*
The 'channel_exec' function executes new process.
If terminal is requsted, than users shell is executed,
if exec is requested, than command 'cmd' is executed.
Process is executed under appropriate users UID.
*/
int channel_exec(const char *cmd) {

    char *run[4];
    char *shell;
    char *name;
    int fd[3];
    char ln[NAME_MAX + 2];

    if (channel.maxpacket == 0) bug_proto();
    if (channel.pid != 0 ) bug_proto();

    if (channel.flagterminal) {
        channel.pid = channel_forkpty(fd, channel.master, channel.slave);
        if (channel.pid > 0) {
            name = ptsname(fd[0]);
            if (!name) bug();
            if (!str_copyn(channel.termname, sizeof channel.termname, name)) bug_nomem();
        }
    }
    else {
        channel.pid = channel_fork(fd);
    }
    if (channel.pid == -1) return 0;
    if (channel.pid == 0) {
        logsys_login(channel.user, channel.remoteip, 0, 0);
        if (!channel_droppriv(channel.user, &shell)) _exit(111);
        if (cmd) {
            run[0] = shell;
            run[1] = (char *)"-c";
            run[2] = (char *)cmd;
            run[3] = 0;
        }
        else {
            if (!loginshell(ln, sizeof ln, shell)) bug();
            run[0] = ln;
            run[1] = 0;
        }
        signal(SIGPIPE, SIG_DFL);
        newenv_exec(shell, run);
        _exit(111);
    }
    channel.fd0 = fd[0];
    channel.fd1 = fd[1];
    channel.fd2 = fd[2];
    channel.len0 = 0;
    newenv_purge();
    if (channel.flagterminal && channel.pid > 0) channel_ptyresize(channel.a, channel.b, channel.x, channel.y);
    return 1;
}

/*
The 'channel_put' function adds data from
client to childs buffer.
*/
void channel_put(unsigned char *buf, long long len) {

    if (channel.maxpacket == 0) bug_proto();
    if (channel.pid <= 0 ) bug_proto();
    if (channel.fd0 == -1) bug_proto();

    if (!buf || len < 0) bug_inval();
    if (channel.len0 + len > CHANNEL_BUFSIZE) bug_nomem();

    byte_copy(channel.buf0 + channel.len0, len, buf);
    channel.len0 += len;
    channel.localwindow -= len;
}

/*
The 'channel_puteof' function adds information
that remote side closed standard output.
*/
void channel_puteof(void) {

    if (channel.maxpacket == 0) bug_proto();
    if (channel.pid == 0) bug_proto();
    if (channel.fd0 == -1) bug_proto();

    channel.remoteeof = 1;
    if (channel.len0 == 0) {
        close(channel.fd0);
        channel.fd0 = -1;
    }
}

/*
The 'channel_putisready' function returns
if child is ready accept data.
*/
int channel_putisready(void) {

    if (channel.maxpacket == 0) return 0;
    if (channel.pid <= 0) return 0;
    if (channel.fd0 == -1) return 0;

    return (CHANNEL_BUFSIZE > channel.len0);
}

/*
The 'channel_read' function reads
data from childs standard output.
*/
long long channel_read(unsigned char *buf, long long len) {

    long long r;

    if (channel.maxpacket == 0) bug_proto();
    if (channel.pid <= 0 ) bug_proto();
    if (channel.fd1 == -1) bug_proto();

    if (!buf || len < 0) bug_inval();

    if (channel.remotewindow <= 0) return 0;

    r = len;
    if (r > 1048576) r = 1048576;
    if (r > channel.remotewindow) r = channel.remotewindow;
    r = read(channel.fd1, buf, r);
    if (r == -1) {
        if (errno == EINTR) return 0;
        if (errno == EAGAIN) return 0;
        if (errno == EWOULDBLOCK) return 0;
    }
    if (r <= 0) {
        channel.fd1 = -1;
        return 0;
    }
    channel.remotewindow -= r;
    return r;
}

/*
The 'channel_extendedread' function reads
data from childs error output.
*/
long long channel_extendedread(unsigned char *buf, long long len) {

    long long r;

    if (channel.maxpacket == 0) bug_proto();
    if (channel.pid <= 0 ) bug_proto();
    if (channel.fd2 == -1) bug_proto();

    if (!buf || len < 0) bug_inval();

    if (channel.remotewindow <= 0) return 0;

    r = len;
    if (r > 1048576) r = 1048576;
    if (r > channel.remotewindow) r = channel.remotewindow;
    r = read(channel.fd2, buf, r);
    if (r == -1) {
        if (errno == EINTR) return 0;
        if (errno == EAGAIN) return 0;
        if (errno == EWOULDBLOCK) return 0;
    }
    if (r <= 0) {
        channel.fd2 = -1;
        return 0;
    }
    channel.remotewindow -= r;
    return r;
}

/*
The 'channel_readisready' function returns
if we can read data from childs standard output.
*/
int channel_readisready(void) {

    if (channel.maxpacket == 0 || channel.pid == 0) return 0;
    if (channel.fd1 == -1) return 0;
    return (channel.remotewindow > 0);
}

/*
The 'channel_extendedreadisready' function returns
if we can read data from childs error output.
*/
int channel_extendedreadisready(void) {

    if (channel.maxpacket == 0 || channel.pid == 0) return 0;
    if (channel.fd2 == -1) return 0;
    return (channel.remotewindow > 0);
}

/*
The 'channel_write' function writes
data to childs standard input.
*/
int channel_write(void) {

    long long w;

    if (channel.maxpacket == 0) bug_proto();
    if (channel.pid <= 0 ) bug_proto();
    if (channel.fd0 == -1) bug_proto();

    if (channel.len0 <= 0) return 1;

    w = write(channel.fd0, channel.buf0, channel.len0);
    if (w == -1) {
        if (errno == EINTR) return 1;
        if (errno == EAGAIN) return 1;
        if (errno == EWOULDBLOCK) return 1;
    }
    if (w <= 0) {
        channel.fd0 = -1;
        return 0;
    }
    byte_copy(channel.buf0, channel.len0 - w, channel.buf0 + w);
    purge(channel.buf0 + channel.len0 - w, w);
    channel.len0 -= w;
    if (channel.remoteeof && channel.len0 == 0) { close(channel.fd0); channel.fd0 = -1; }
    return 1;
}

/*
The 'channel_writeisready' function returns
if we can write data to childs standard input.
*/
int channel_writeisready(void) {

    if (channel.maxpacket == 0) return 0;
    if (channel.pid <= 0 ) return 0;
    if (channel.fd0 == -1) return 0;

    return (channel.len0 > 0);
}

/*
The 'channel_iseof' function returns
if child closed standard and error output.
*/
int channel_iseof(void) {

    if (channel.maxpacket == 0) return 0;
    if (channel.pid == 0) return 0;
    if (channel.pid == -1) return 1;
    return (channel.fd1 == -1 && channel.fd2 == -1);
}

/*
The 'channel_waitnohang' function returns
if child exited or was killed.
*/
int channel_waitnohang(int *s, int *e) {

    int r, status;


    if (!s || !e) bug_inval();
    if (channel.maxpacket == 0) bug_proto();
    if (channel.pid <= 0 ) return 0;

    do {
        r = waitpid(channel.pid, &status, WNOHANG);
    } while (r == -1 && errno == EINTR);
    if (r <= 0) return 0;

    if (channel.flagterminal) logsys_logout(channel.user, channel.remoteip, channel.termname, channel.pid);

    if (WIFEXITED(status)) {
        *e = WEXITSTATUS(status);
        *s = 0;
    }
    else if (WIFSIGNALED(status)) {
        *e = 0;
        *s = WTERMSIG(status);
    }
    else {
        *e = *s = -1;
    }

    channel.pid = -1;
    return 1;
}

/*
Remove sentitive data from allocated memory.
*/
void channel_purge(void) {
    purge(&channel, sizeof channel);
    trymunlock(&channel, sizeof channel);
}

/*
Initialize channel structure.
*/
void channel_init(void) {
    trymlock(&channel, sizeof channel);
    purge(&channel, sizeof channel);
    channel.maxpacket = 0;
    channel.remoteeof = 0;
    channel.len0 = 0;
    channel.pid = 0;
    channel.flagterminal = 0;
    channel.master = -1;
    channel.slave = -1;
}

int channel_getfd0(void) { return channel.fd0; }
int channel_getfd1(void) { return channel.fd1; }
int channel_getfd2(void) { return channel.fd2; }
long long channel_getlen0(void) { return channel.len0; }
crypto_uint32 channel_getid(void) { return channel.id; }
crypto_uint32 channel_getlocalwindow(void) { return channel.localwindow; }
void channel_incrementremotewindow(crypto_uint32 x) { channel.remotewindow += x; }
void channel_incrementlocalwindow(crypto_uint32 x) { channel.localwindow += x; }
