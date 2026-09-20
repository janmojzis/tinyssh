/*
20260905
Public domain.
*/

#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include "buf.h"
#include "channel.h"
#include "crypto_uint32.h"
#include "global.h"
#include "packet.h"
#include "ssh.h"

static unsigned char space1[2048];
static unsigned char space2[2048];
static struct buf b1;
static struct buf b2;

static crypto_uint32 load32(const unsigned char *x) {
    return crypto_uint32_load_bigendian(x);
}

static int replytype(crypto_uint8 type) {
    return packet.sendbuf.len >= 10 && packet.sendbuf.buf[5] == type;
}

static void reset(void) {
    packet_init();
    channel_init();
    buf_init(&b1, space1, sizeof space1);
    buf_init(&b2, space2, sizeof space2);
}

static void openrequest(const char *type, crypto_uint32 id,
                        crypto_uint32 window, crypto_uint32 maxpacket,
                        int trailing) {
    buf_purge(&b1);
    buf_putnum8(&b1, SSH_MSG_CHANNEL_OPEN);
    buf_putstring(&b1, type);
    buf_putnum32(&b1, id);
    buf_putnum32(&b1, window);
    buf_putnum32(&b1, maxpacket);
    if (trailing) buf_putnum8(&b1, 0);
}

static int opentests(void) {
    reset();
    packet.name[0] = 'u';
    packet.name[1] = 0;
    openrequest("session", 42, 12345, 1, 0);
    if (!packet_channel_open(&b1, &b2)) return 0;
    if (!replytype(SSH_MSG_CHANNEL_OPEN_CONFIRMATION)) return 0;
    if (channel.id != 42 || channel.remotewindow != 12345 ||
        channel.maxpacket != 32 || channel.localwindow != CHANNEL_BUFSIZE)
        return 0;
    if (load32(packet.sendbuf.buf + 6) != 42 ||
        load32(packet.sendbuf.buf + 10) != 42 ||
        load32(packet.sendbuf.buf + 14) != CHANNEL_BUFSIZE ||
        load32(packet.sendbuf.buf + 18) != CHANNEL_PACKET_LIMIT)
        return 0;

    buf_purge(&packet.sendbuf);
    openrequest("session", 99, 1, CHANNEL_PACKET_LIMIT + 100, 0);
    if (!packet_channel_open(&b1, &b2)) return 0;
    if (!replytype(SSH_MSG_CHANNEL_OPEN_FAILURE)) return 0;
    if (load32(packet.sendbuf.buf + 6) != 99 ||
        load32(packet.sendbuf.buf + 10) != SSH_OPEN_ADMINISTRATIVELY_PROHIBITED)
        return 0;

    reset();
    openrequest("direct-tcpip", 7, 1, 1024, 1);
    if (!packet_channel_open(&b1, &b2)) return 0;
    return replytype(SSH_MSG_CHANNEL_OPEN_FAILURE) &&
           load32(packet.sendbuf.buf + 6) == 7 &&
           load32(packet.sendbuf.buf + 10) == SSH_OPEN_UNKNOWN_CHANNEL_TYPE;
}

static void requestprefix(const char *type, crypto_uint8 wantreply) {
    buf_purge(&b1);
    buf_putnum8(&b1, SSH_MSG_CHANNEL_REQUEST);
    buf_putnum32(&b1, 42);
    buf_putstring(&b1, type);
    buf_putnum8(&b1, wantreply);
}

static int request(const char *type, crypto_uint8 wantreply,
                   crypto_uint8 expected) {
    reset();
    channel.id = 42;
    channel.maxpacket = 1024;
    requestprefix(type, wantreply);
    if (!packet_channel_request(&b1, &b2, 0)) return 0;
    if (!wantreply) return packet.sendbuf.len == 0;
    return replytype(expected) && load32(packet.sendbuf.buf + 6) == 42;
}

static int execandsubsystem(void) {
    reset();
    channel.id = 42;
    channel.maxpacket = 1024;
    requestprefix("exec", 1);
    buf_putstring(&b1, "true");
    if (!packet_channel_request(&b1, &b2, "/bin/false")) return 0;
    if (!replytype(SSH_MSG_CHANNEL_FAILURE)) return 0;

    buf_purge(&packet.sendbuf);
    requestprefix("subsystem", 1);
    buf_putstring(&b1, "missing");
    if (!packet_channel_request(&b1, &b2, 0)) return 0;
    return replytype(SSH_MSG_CHANNEL_FAILURE);
}

static int statefulrequests(void) {
    reset();
    channel.id = 42;
    channel.maxpacket = 1024;
    channel.pid = 1;
    requestprefix("shell", 1);
    if (!packet_channel_request(&b1, &b2, 0) ||
        !replytype(SSH_MSG_CHANNEL_FAILURE))
        return 0;

    buf_purge(&packet.sendbuf);
    requestprefix("pty-req", 1);
    buf_putstring(&b1, "xterm");
    buf_putnum32(&b1, 80);
    buf_putnum32(&b1, 24);
    buf_putnum32(&b1, 0);
    buf_putnum32(&b1, 0);
    buf_putstring(&b1, "");
    if (!packet_channel_request(&b1, &b2, 0) ||
        !replytype(SSH_MSG_CHANNEL_FAILURE))
        return 0;

    channel.pid = 0;
    buf_purge(&packet.sendbuf);
    requestprefix("window-change", 1);
    buf_putnum32(&b1, 80);
    buf_putnum32(&b1, 24);
    buf_putnum32(&b1, 0);
    buf_putnum32(&b1, 0);
    if (!packet_channel_request(&b1, &b2, 0)) return 0;
    return replytype(SSH_MSG_CHANNEL_FAILURE);
}

static int envrequest(int embeddedzero, const char *customcmd) {
    static const unsigned char badname[] = {'L', 'A', 'N', 'G', 0, 'X'};

    reset();
    channel.id = 42;
    channel.maxpacket = 1024;
    requestprefix("env", 1);
    if (embeddedzero)
        buf_putstringlen(&b1, badname, sizeof badname);
    else
        buf_putstring(&b1, "LANG");
    buf_putstring(&b1, "C");
    if (!packet_channel_request(&b1, &b2, customcmd)) return 0;
    return replytype(SSH_MSG_CHANNEL_FAILURE);
}

static int preexecclose(void) {
    int fd[2];

    reset();
    channel.id = 42;
    channel.maxpacket = 1024;
    if (pipe(fd) == -1) return 0;
    channel.master = fd[0];
    channel.slave = fd[1];
    channel.flagterminal = 1;

    buf_purge(&b1);
    buf_putnum8(&b1, SSH_MSG_CHANNEL_CLOSE);
    buf_putnum32(&b1, 42);
    if (!packet_channel_recv_close(&b1)) return 0;
    if (channel.pid != -1 || channel.master != -1 || channel.slave != -1 ||
        channel.flagterminal)
        return 0;
    if (fcntl(fd[0], F_GETFD) != -1 || fcntl(fd[1], F_GETFD) != -1) return 0;
    if (!packet.flagchanneleofreceived || !packet.flagclosesent) return 0;

    /* no child descriptor may be left for the main loop to watch, otherwise
       it polls descriptor 0 (the network) as the child's output */
    if (channel_getfd0() != -1 || channel_getfd1() != -1 ||
        channel_getfd2() != -1)
        return 0;
    if (channel_writeisready() || channel_readisready() ||
        channel_extendedreadisready())
        return 0;

    buf_purge(&b1);
    buf_putnum8(&b1, SSH_MSG_CHANNEL_CLOSE);
    buf_putnum32(&b1, 42);
    if (!packet_channel_recv_close(&b1) || channel.pid != -1) return 0;

    buf_purge(&packet.sendbuf);
    requestprefix("shell", 1);
    if (!packet_channel_request(&b1, &b2, 0)) return 0;
    return channel.pid == -1 && replytype(SSH_MSG_CHANNEL_FAILURE);
}

static int activeclose(int pending) {
    unsigned char ch = 'x', out;
    long long r;
    int fd[2];

    reset();
    channel.id = 42;
    channel.maxpacket = 1024;
    channel.pid = 1;
    channel.localwindow = CHANNEL_BUFSIZE;
    if (pipe(fd) == -1) return 0;
    channel.fd0 = fd[1];
    if (pending) channel_put(&ch, 1);

    buf_purge(&b1);
    buf_putnum8(&b1, SSH_MSG_CHANNEL_CLOSE);
    buf_putnum32(&b1, 42);
    if (!packet_channel_recv_close(&b1)) return 0;
    if (!channel.remoteeof) return 0;
    if (pending) {
        if (channel.fd0 == -1 || channel.len0 != 1) return 0;
        if (!channel_write() || channel.fd0 != -1 || channel.len0 != 0)
            return 0;
        r = read(fd[0], &out, 1);
        if (r != 1 || out != ch) return 0;
    }
    else if (channel.fd0 != -1 || channel.len0 != 0)
        return 0;
    r = read(fd[0], &out, 1);
    close(fd[0]);
    return r == 0;
}

enum fatalmode {
    WRONG_ID,
    SHELL_TRAILING,
    TRUNCATED_REQUEST,
    SESSION_TRAILING
};

static int fatal(enum fatalmode mode) {
    int status;
    pid_t pid = fork();

    if (pid == -1) return 0;
    if (pid == 0) {
        global_init();
        buf_init(&b1, space1, sizeof space1);
        buf_init(&b2, space2, sizeof space2);
        channel.id = 42;
        channel.maxpacket = 1024;
        if (mode == SESSION_TRAILING) {
            packet.name[0] = 'u';
            packet.name[1] = 0;
            openrequest("session", 42, 1, 1024, 1);
            packet_channel_open(&b1, &b2);
        }
        else {
            requestprefix("shell", 1);
            if (mode == WRONG_ID) crypto_uint32_store_bigendian(b1.buf + 1, 43);
            if (mode == SHELL_TRAILING) buf_putnum8(&b1, 0);
            if (mode == TRUNCATED_REQUEST) b1.len = 3;
            packet_channel_request(&b1, &b2, 0);
        }
        _exit(0);
    }
    if (waitpid(pid, &status, 0) != pid) return 0;
    return WIFEXITED(status) && WEXITSTATUS(status) == 111;
}

int main(void) {
    int ok = 1;

    global_init();
    buf_init(&b1, space1, sizeof space1);
    buf_init(&b2, space2, sizeof space2);
    if (!opentests()) ok = 0;
    if (!request("unknown@example", 1, SSH_MSG_CHANNEL_FAILURE)) ok = 0;
    if (!request("unknown@example", 0, 0)) ok = 0;
    if (!execandsubsystem()) ok = 0;
    if (!statefulrequests()) ok = 0;
    if (!envrequest(0, 0)) ok = 0;
    if (!envrequest(1, 0)) ok = 0;
    if (!envrequest(0, "true")) ok = 0;
    if (!preexecclose()) ok = 0;
    if (!activeclose(0)) ok = 0;
    if (!activeclose(1)) ok = 0;
    if (!fatal(WRONG_ID)) ok = 0;
    if (!fatal(SHELL_TRAILING)) ok = 0;
    if (!fatal(TRUNCATED_REQUEST)) ok = 0;
    if (!fatal(SESSION_TRAILING)) ok = 0;
    global_purge();
    return ok ? 0 : 111;
}
