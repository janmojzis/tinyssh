/*
20140107
Jan Mojzis
Public domain.
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <poll.h>
#include "blocking.h"
#include "ssh.h"
#include "purge.h"
#include "open.h"
#include "load.h"
#include "e.h"
#include "byte.h"
#include "buf.h"
#include "packet.h"
#include "channel.h"
#include "log.h"
#include "sshcrypto.h"
#include "subprocess.h"
#include "global.h"
#include "connectioninfo.h"
#include "die.h"
#include "main.h"

#define USAGE "usage: tinysshd [options] keydir"

static unsigned int cryptotypeselected = sshcrypto_TYPENEWCRYPTO;
static int flagverbose = 1;
static int fdwd;
static int flaglogger = 0;

static struct buf b1 = {global_bspace1, 0, sizeof global_bspace1};
static struct buf b2 = {global_bspace2, 0, sizeof global_bspace2};

static void timeout(int x) {
    errno = x = ETIMEDOUT;
    die_fatal("closing connection", 0, 0);
}

static int selfpipe[2] = { -1, -1 };

static void trigger(int x) {
    errno = 0;
    x = write(selfpipe[1], "", 1);
}


int main_tinysshd(int argc, char **argv) {

    char *x;
    const char *keydir = 0;
    long long i;
    struct pollfd p[6];
    struct pollfd *q;
    struct pollfd *watch0;
    struct pollfd *watch1;
    struct pollfd *watchtochild;
    struct pollfd *watchfromchild1;
    struct pollfd *watchfromchild2;
    struct pollfd *watchselfpipe;
    int exitsignal, exitcode;

    signal(SIGPIPE, SIG_IGN);
    signal(SIGALRM, timeout);

    log_init(0, "tinysshd", 0, 0);

    if (argc < 2) die_usage(USAGE);
    if (!argv[0]) die_usage(USAGE);
    for (;;) {
        if (!argv[1]) break;
        if (argv[1][0] != '-') break;
        x = *++argv;
        if (x[0] == '-' && x[1] == 0) break;
        if (x[0] == '-' && x[1] == '-' && x[2] == 0) break;
        while (*++x) {
            if (*x == 'q') { flagverbose = 0; continue; }
            if (*x == 'Q') { flagverbose = 1; continue; }
            if (*x == 'v') { if (flagverbose >= 2) flagverbose = 3; else flagverbose = 2; continue; }
            if (*x == 'o') { cryptotypeselected |= sshcrypto_TYPEOLDCRYPTO; continue; }
            if (*x == 'O') { cryptotypeselected &= ~sshcrypto_TYPEOLDCRYPTO; continue; }
            if (*x == 's') { cryptotypeselected |= sshcrypto_TYPENEWCRYPTO; continue; }
            if (*x == 'S') { cryptotypeselected &= ~sshcrypto_TYPENEWCRYPTO; continue; }
            if (*x == 'p') { cryptotypeselected |= sshcrypto_TYPEPQCRYPTO; continue; }
            if (*x == 'P') { cryptotypeselected &= ~sshcrypto_TYPEPQCRYPTO; continue; }
            if (*x == 'l') { flaglogger = 1; continue; }
            if (*x == 'L') { flaglogger = 0; continue; }
            if (*x == 'x') {
                if (x[1]) { channel_subsystem_add(x + 1); break; }
                if (argv[1]) { channel_subsystem_add(*++argv); break; }
            }

            die_usage(USAGE);
        }
    }
    keydir = *++argv; if (!keydir) die_usage(USAGE);

    log_init(flagverbose, "tinysshd", 1, flaglogger);

    connectioninfo(channel.localip, channel.localport, channel.remoteip, channel.remoteport);
    log_i4("connection from ", channel.remoteip, ":", channel.remoteport);

    channel_subsystem_log();

    global_init();

    blocking_disable(0);
    blocking_disable(1);
    blocking_disable(2);

    /* get server longterm keys */
    fdwd = open_cwd();
    if (fdwd == -1) die_fatal("unable to open current directory", 0, 0);
    if (chdir(keydir) == -1) die_fatal("unable to chdir to", keydir, 0);

    for (i = 0; sshcrypto_keys[i].name; ++i) sshcrypto_keys[i].sign_flagserver |= sshcrypto_keys[i].cryptotype & cryptotypeselected;
    for (i = 0; sshcrypto_keys[i].name; ++i) sshcrypto_keys[i].sign_flagclient |= sshcrypto_keys[i].cryptotype & cryptotypeselected;
    for (i = 0; sshcrypto_kexs[i].name; ++i) sshcrypto_kexs[i].flagenabled |= sshcrypto_kexs[i].cryptotype & cryptotypeselected;
    for (i = 0; sshcrypto_ciphers[i].name; ++i) sshcrypto_ciphers[i].flagenabled |= sshcrypto_ciphers[i].cryptotype & cryptotypeselected;

    /* read public keys */
    for (i = 0; sshcrypto_keys[i].name; ++i) {
        if (!sshcrypto_keys[i].sign_flagserver) continue;
        if (load(sshcrypto_keys[i].sign_publickeyfilename, sshcrypto_keys[i].sign_publickey, sshcrypto_keys[i].sign_publickeybytes) == -1) {
            sshcrypto_keys[i].sign_flagserver = 0;
            if (errno == ENOENT) continue;
            die_fatal("unable to read public key from file", keydir, sshcrypto_keys[i].sign_publickeyfilename);
        }
    }

    if (fchdir(fdwd) == -1) die_fatal("unable to change directory to working directory", 0, 0);
    close(fdwd);

    /* set timeout */
    alarm(60);

    /* send and receive hello */
    if (!packet_hello_send()) die_fatal("unable to send hello-string", 0, 0);
    if (!packet_hello_receive()) die_fatal("unable to receive hello-string", 0, 0);

    /* send and receive kex */
    if (!packet_kex_send()) die_fatal("unable to send kex-message", 0, 0);
    if (!packet_kex_receive()) die_fatal("unable to receive kex-message", 0, 0);

rekeying:
    /* rekeying */
    alarm(60);
    if (packet.flagrekeying == 1) {
        buf_purge(&packet.kexrecv);
        buf_put(&packet.kexrecv, b1.buf, b1.len);
        if (!packet_kex_send()) die_fatal("unable to send kex-message", 0, 0);
    }

    /* send and receive kexdh */
    if (!packet_kexdh(keydir, &b1, &b2)) die_fatal("unable to subprocess kexdh", 0, 0);

    if (packet.flagkeys) log_d1("rekeying: done");
    packet.flagkeys = 1;

    /* note: communication is encrypted */

    /* authentication + authorization */
    if (packet.flagauthorized == 0) {
        if (!packet_auth(&b1, &b2)) die_fatal("authentication failed", 0, 0);
        packet.flagauthorized = 1;
    }

    /* note: user is authenticated and authorized */
    alarm(3600);

    /* main loop */
    for (;;) {
        if (channel_iseof())
            if (!packet.sendbuf.len)
                if (packet.flagchanneleofreceived)
                    break;

        watch0 = watch1 = 0;
        watchtochild = watchfromchild1 = watchfromchild2 = 0;
        watchselfpipe = 0;

        q = p;

        if (packet_sendisready()) { watch1 = q; q->fd = 1; q->events = POLLOUT; ++q; }
        if (packet_recvisready()) { watch0 = q; q->fd = 0; q->events = POLLIN;  ++q; }

        if (channel_writeisready()) { watchtochild = q; q->fd = channel_getfd0(); q->events = POLLOUT; ++q; }
        if (channel_readisready() && packet_putisready()) { watchfromchild1 = q; q->fd = channel_getfd1(); q->events = POLLIN; ++q; }
        if (channel_extendedreadisready() && packet_putisready()) { watchfromchild2 = q; q->fd = channel_getfd2(); q->events = POLLIN; ++q; }

        if (selfpipe[0] != -1) { watchselfpipe = q; q->fd = selfpipe[0]; q->events = POLLIN; ++q; }

        if (poll(p, q - p, 60000) < 0) {
            watch0 = watch1 = 0;
            watchtochild = watchfromchild1 = watchfromchild2 = 0;
            watchselfpipe = 0;
        }

        else {
            if (watch0) if (!watch0->revents) watch0 = 0;
            if (watch1) if (!watch1->revents) watch1 = 0;
            if (watchfromchild1) if (!watchfromchild1->revents) watchfromchild1 = 0;
            if (watchfromchild2) if (!watchfromchild2->revents) watchfromchild2 = 0;
            if (watchtochild) if (!watchtochild->revents) watchtochild = 0;
            if (watchselfpipe) if (!watchselfpipe->revents) watchselfpipe = 0;
        }

        if (watchtochild) {

            /* write data to child */
            if (!channel_write()) die_fatal("unable to write data to child", 0, 0);

            /* try to adjust window */
            if (!packet_channel_send_windowadjust(&b1)) die_fatal("unable to send data to network", 0, 0);
        }

        /* read data from child */
        if (watchfromchild1) packet_channel_send_data(&b2);
        if (watchfromchild2) packet_channel_send_extendeddata(&b2);

        /* check child */
        if (channel_iseof()) {
            if (selfpipe[0] == -1) if (open_pipe(selfpipe) == -1) die_fatal("unable to open pipe", 0, 0);
            signal(SIGCHLD, trigger);
            if (channel_waitnohang(&exitsignal, &exitcode)) {
                packet_channel_send_eof(&b2);
                if (!packet_channel_send_close(&b2, exitsignal, exitcode)) die_fatal("unable to close channel", 0, 0);
            }
        }

        /* send data to network */
        if (watch1) if (!packet_send()) die_fatal("unable to send data to network", 0, 0);

        /* receive data from network */
        if (watch0) {
            alarm(3600); /* refresh timeout */
            if (!packet_recv()) {
                if (channel_iseof()) break; /* XXX */
                die_fatal("unable to receive data from network", 0, 0);
            }
        }

        /* process packets */
        for (;;) {

            if (!packet_get(&b1, 0)) {
                if (!errno) break;
                die_fatal("unable to get packets from network", 0, 0);
            }
            if (b1.len < 1) break; /* XXX */

            switch (b1.buf[0]) {
                case SSH_MSG_CHANNEL_OPEN:
                    if (!packet_channel_open(&b1, &b2)) die_fatal("unable to open channel", 0, 0);
                    break;
                case SSH_MSG_CHANNEL_REQUEST:
                    if (!packet_channel_request(&b1, &b2)) die_fatal("unable to handle channel-request", 0, 0);
                    break;
                case SSH_MSG_CHANNEL_DATA:
                    if (!packet_channel_recv_data(&b1)) die_fatal("unable to handle channel-data", 0, 0);
                    break;
                case SSH_MSG_CHANNEL_EXTENDED_DATA:
                    if (!packet_channel_recv_extendeddata(&b1)) die_fatal("unable to handle channel-extended-data", 0, 0);
                    break;
                case SSH_MSG_CHANNEL_WINDOW_ADJUST:
                    if (!packet_channel_recv_windowadjust(&b1)) die_fatal("unable to handle channel-window-adjust", 0, 0);
                    break;
                case SSH_MSG_CHANNEL_EOF:
                    if (!packet_channel_recv_eof(&b1)) die_fatal("unable to handle channel-eof", 0, 0);
                    break;
                case SSH_MSG_CHANNEL_CLOSE:
                    if (!packet_channel_recv_close(&b1)) die_fatal("unable to handle channel-close", 0, 0);
                    break;
                case SSH_MSG_KEXINIT:
                    goto rekeying;
                default:
                    if (!packet_unimplemented(&b1)) die_fatal("unable to send SSH_MSG_UNIMPLEMENTED message", 0, 0);
            }
        }
    }

    log_i1("finished");
    global_die(0); return 111;
}
