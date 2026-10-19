/*
20260905
Public domain.
*/

#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "buf.h"
#include "channel.h"
#include "crypto_uint32.h"
#include "global.h"
#include "packet.h"
#include "ssh.h"

static unsigned char space[TRANSPORT_PACKET_LIMIT + 1];
static struct buf b;

static void reset(crypto_uint32 window, long long pid) {
    packet_init();
    channel_init();
    buf_init(&b, space, sizeof space);
    channel.id = 42;
    channel.maxpacket = 1024;
    channel.localwindow = window;
    channel.remotewindow = window;
    channel.pid = pid;
    channel.fd0 = -1;
    channel.fd1 = -1;
    channel.fd2 = -1;
}

static void datarequest(crypto_uint8 type, crypto_uint32 id,
                        crypto_uint32 datatype, const unsigned char *data,
                        long long len, int trailing) {
    buf_purge(&b);
    buf_putnum8(&b, type);
    buf_putnum32(&b, id);
    if (type == SSH_MSG_CHANNEL_EXTENDED_DATA) buf_putnum32(&b, datatype);
    buf_putstringlen(&b, data, len);
    if (trailing) buf_putnum8(&b, 0);
}

static int receivedata(void) {
    static const unsigned char text[] = "hello";
    int p[2];

    reset(100, 1);
    if (pipe(p) == -1) return 0;
    close(p[0]);
    channel.fd0 = p[1];
    datarequest(SSH_MSG_CHANNEL_DATA, 42, 0, text, sizeof text - 1, 0);
    if (!packet_channel_recv_data(&b)) return 0;
    close(channel.fd0);
    channel.fd0 = -1;
    return channel.len0 == sizeof text - 1 && channel.localwindow == 95 &&
           b.len == 0;
}

/* discarded data is charged to the window */
static int receivedatadiscarded(void) {
    static const unsigned char text[] = "hello";

    reset(100, 1);
    datarequest(SSH_MSG_CHANNEL_DATA, 42, 0, text, sizeof text - 1, 0);
    if (!packet_channel_recv_data(&b)) return 0;
    if (channel.len0 != 0 || channel.localwindow != 95 || b.len != 0) return 0;

    reset(100, -1);
    datarequest(SSH_MSG_CHANNEL_DATA, 42, 0, text, sizeof text - 1, 0);
    if (!packet_channel_recv_data(&b)) return 0;
    return channel.len0 == 0 && channel.localwindow == 95 && b.len == 0;
}

static int receiveextended(void) {
    static const unsigned char text[] = "stderr";

    reset(100, 1);
    datarequest(SSH_MSG_CHANNEL_EXTENDED_DATA, 42, 1, text, sizeof text - 1, 0);
    if (!packet_channel_recv_extendeddata(&b)) return 0;
    return channel.len0 == 0 && channel.localwindow == 94 && b.len == 0;
}

static int receivepacketlimit(void) {
    static const unsigned char data[CHANNEL_PACKET_LIMIT];

    reset(CHANNEL_BUFSIZE, 1);
    channel.fd0 = 0;
    datarequest(SSH_MSG_CHANNEL_DATA, 42, 0, data, sizeof data, 0);
    if (!packet_channel_recv_data(&b)) return 0;
    if (channel.len0 != CHANNEL_PACKET_LIMIT ||
        channel.localwindow != CHANNEL_BUFSIZE - CHANNEL_PACKET_LIMIT)
        return 0;

    reset(CHANNEL_BUFSIZE, 1);
    datarequest(SSH_MSG_CHANNEL_EXTENDED_DATA, 42, 1, data, sizeof data, 0);
    if (!packet_channel_recv_extendeddata(&b)) return 0;
    return channel.localwindow == CHANNEL_BUFSIZE - CHANNEL_PACKET_LIMIT;
}

static int senddatalimit(void) {
    static unsigned char data[100];
    int p[2];

    reset(100, 1);
    channel.maxpacket = 32;
    if (pipe(p) == -1) return 0;
    if (write(p[1], data, sizeof data) != (long long) sizeof data) return 0;
    close(p[1]);
    channel.fd1 = p[0];
    packet_channel_send_data(&b);
    close(channel.fd1);
    channel.fd1 = -1;
    return packet.sendbuf.buf[5] == SSH_MSG_CHANNEL_DATA &&
           crypto_uint32_load_bigendian(packet.sendbuf.buf + 10) == 32 &&
           channel.remotewindow == 68;
}

static int sendextendedlimit(void) {
    static unsigned char data[100];
    int p[2];

    reset(100, 1);
    channel.maxpacket = 32;
    if (pipe(p) == -1) return 0;
    if (write(p[1], data, sizeof data) != (long long) sizeof data) return 0;
    close(p[1]);
    channel.fd2 = p[0];
    packet_channel_send_extendeddata(&b);
    close(channel.fd2);
    channel.fd2 = -1;
    return packet.sendbuf.buf[5] == SSH_MSG_CHANNEL_EXTENDED_DATA &&
           crypto_uint32_load_bigendian(packet.sendbuf.buf + 14) == 32 &&
           channel.remotewindow == 68;
}

static int windowadjust(void) {
    reset(100, 1);
    buf_purge(&b);
    buf_putnum8(&b, SSH_MSG_CHANNEL_WINDOW_ADJUST);
    buf_putnum32(&b, 42);
    buf_putnum32(&b, 50);
    if (!packet_channel_recv_windowadjust(&b) || channel.remotewindow != 150)
        return 0;

    buf_purge(&b);
    buf_putnum8(&b, SSH_MSG_CHANNEL_WINDOW_ADJUST);
    buf_putnum32(&b, 42);
    buf_putnum32(&b, 0);
    return packet_channel_recv_windowadjust(&b) && channel.remotewindow == 150;
}

static int sendwindowadjust(void) {
    reset(10, 1);
    channel.len0 = 20;
    if (!packet_channel_send_windowadjust(&b)) return 0;
    if (channel.localwindow != CHANNEL_BUFSIZE - 20) return 0;
    if (packet.sendpacketid != 1 ||
        packet.sendbuf.buf[5] != SSH_MSG_CHANNEL_WINDOW_ADJUST)
        return 0;
    if (crypto_uint32_load_bigendian(packet.sendbuf.buf + 6) != 42) return 0;

    buf_purge(&packet.sendbuf);
    if (!packet_channel_send_windowadjust(&b)) return 0;
    return packet.sendpacketid == 1 && packet.sendbuf.len == 0;
}

static void idpacket(crypto_uint8 type, crypto_uint32 id, int trailing) {
    buf_purge(&b);
    buf_putnum8(&b, type);
    buf_putnum32(&b, id);
    if (trailing) buf_putnum8(&b, 0);
}

static int eofandclose(void) {
    crypto_uint32 firstlen;

    reset(100, 0);
    idpacket(SSH_MSG_CHANNEL_EOF, 42, 0);
    if (!packet_channel_recv_eof(&b) || !channel.remoteeof) return 0;
    idpacket(SSH_MSG_CHANNEL_EOF, 42, 0);
    if (!packet_channel_recv_eof(&b) || !channel.remoteeof) return 0;

    idpacket(SSH_MSG_CHANNEL_CLOSE, 42, 0);
    if (!packet_channel_recv_close(&b)) return 0;
    if (!packet.flageofsent || !packet.flagclosesent ||
        !packet.flagchanneleofreceived || packet.sendpacketid != 2)
        return 0;
    if (packet.sendbuf.buf[5] != SSH_MSG_CHANNEL_EOF) return 0;
    firstlen = crypto_uint32_load_bigendian(packet.sendbuf.buf) + 4;
    if (packet.sendbuf.buf[firstlen + 5] != SSH_MSG_CHANNEL_CLOSE) return 0;

    idpacket(SSH_MSG_CHANNEL_CLOSE, 42, 0);
    if (!packet_channel_recv_close(&b)) return 0;
    return packet.sendpacketid == 2;
}

enum fatalmode {
    DATA_WINDOW,
    DATA_WINDOW_DISCARDED,
    DATA_ID,
    DATA_TRAILING,
    DATA_PACKET_LIMIT,
    EXTENDED_WINDOW,
    EXTENDED_TRUNCATED,
    EXTENDED_PACKET_LIMIT,
    WINDOW_OVERFLOW,
    EOF_ID,
    CLOSE_TRAILING
};

static int fatal(enum fatalmode mode) {
    static const unsigned char text[] = "0123456789";
    static const unsigned char large[CHANNEL_PACKET_LIMIT + 1];
    int status;
    pid_t pid = fork();

    if (pid == -1) return 0;
    if (pid == 0) {
        global_init();
        buf_init(&b, space, sizeof space);
        reset(5, 1);
        if (mode == DATA_WINDOW || mode == DATA_ID || mode == DATA_TRAILING) {
            datarequest(SSH_MSG_CHANNEL_DATA, mode == DATA_ID ? 43 : 42, 0,
                        text, sizeof text - 1, mode == DATA_TRAILING);
            packet_channel_recv_data(&b);
        }
        if (mode == DATA_WINDOW_DISCARDED) {
            reset(15, -1);
            datarequest(SSH_MSG_CHANNEL_DATA, 42, 0, text, sizeof text - 1, 0);
            if (!packet_channel_recv_data(&b)) _exit(0);
            datarequest(SSH_MSG_CHANNEL_DATA, 42, 0, text, sizeof text - 1, 0);
            packet_channel_recv_data(&b);
        }
        if (mode == DATA_PACKET_LIMIT) {
            reset(CHANNEL_BUFSIZE, 1);
            datarequest(SSH_MSG_CHANNEL_DATA, 42, 0, large, sizeof large, 0);
            packet_channel_recv_data(&b);
        }
        if (mode == EXTENDED_WINDOW || mode == EXTENDED_TRUNCATED) {
            datarequest(SSH_MSG_CHANNEL_EXTENDED_DATA, 42, 1, text,
                        sizeof text - 1, 0);
            if (mode == EXTENDED_TRUNCATED) --b.len;
            packet_channel_recv_extendeddata(&b);
        }
        if (mode == EXTENDED_PACKET_LIMIT) {
            reset(CHANNEL_BUFSIZE, 1);
            datarequest(SSH_MSG_CHANNEL_EXTENDED_DATA, 42, 1, large,
                        sizeof large, 0);
            packet_channel_recv_extendeddata(&b);
        }
        if (mode == WINDOW_OVERFLOW) {
            channel.remotewindow = ~(crypto_uint32) 0;
            buf_purge(&b);
            buf_putnum8(&b, SSH_MSG_CHANNEL_WINDOW_ADJUST);
            buf_putnum32(&b, 42);
            buf_putnum32(&b, 1);
            packet_channel_recv_windowadjust(&b);
        }
        if (mode == EOF_ID) {
            idpacket(SSH_MSG_CHANNEL_EOF, 43, 0);
            packet_channel_recv_eof(&b);
        }
        if (mode == CLOSE_TRAILING) {
            idpacket(SSH_MSG_CHANNEL_CLOSE, 42, 1);
            packet_channel_recv_close(&b);
        }
        _exit(0);
    }
    if (waitpid(pid, &status, 0) != pid) return 0;
    return WIFEXITED(status) && WEXITSTATUS(status) == 111;
}

static int sendclose(void) {
    int status;
    pid_t pid = fork();

    if (pid == -1) return 0;
    if (pid == 0) {
        int fd;
        global_init();
        buf_init(&b, space, sizeof space);
        reset(100, -1);
        fd = open("/dev/null", O_WRONLY);
        if (fd == -1 || dup2(fd, 1) == -1) _exit(100);
        close(fd);
        if (!packet_channel_send_close(&b, 0, 23)) _exit(101);
        if (!packet.flagclosesent || !packet.flageofsent ||
            packet.sendpacketid != 3)
            _exit(102);
        if (!packet_channel_send_close(&b, 0, 23) || packet.sendpacketid != 3)
            _exit(103);
        _exit(0);
    }
    if (waitpid(pid, &status, 0) != pid) return 0;
    return WIFEXITED(status) && WEXITSTATUS(status) == 0;
}

int main(void) {
    int ok = 1;

    global_init();
    buf_init(&b, space, sizeof space);
    if (!receivedata()) ok = 0;
    if (!receivedatadiscarded()) ok = 0;
    if (!receiveextended()) ok = 0;
    if (!receivepacketlimit()) ok = 0;
    if (!senddatalimit()) ok = 0;
    if (!sendextendedlimit()) ok = 0;
    if (!windowadjust()) ok = 0;
    if (!sendwindowadjust()) ok = 0;
    if (!eofandclose()) ok = 0;
    if (!fatal(DATA_WINDOW)) ok = 0;
    if (!fatal(DATA_WINDOW_DISCARDED)) ok = 0;
    if (!fatal(DATA_ID)) ok = 0;
    if (!fatal(DATA_TRAILING)) ok = 0;
    if (!fatal(DATA_PACKET_LIMIT)) ok = 0;
    if (!fatal(EXTENDED_WINDOW)) ok = 0;
    if (!fatal(EXTENDED_TRUNCATED)) ok = 0;
    if (!fatal(EXTENDED_PACKET_LIMIT)) ok = 0;
    if (!fatal(WINDOW_OVERFLOW)) ok = 0;
    if (!fatal(EOF_ID)) ok = 0;
    if (!fatal(CLOSE_TRAILING)) ok = 0;
    if (!sendclose()) ok = 0;
    global_purge();
    return ok ? 0 : 111;
}
