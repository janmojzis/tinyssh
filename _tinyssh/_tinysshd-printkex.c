/*
20141210
Jan Mojzis
Public domain.
*/

#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include "log.h"
#include "packet.h"
#include "ssh.h"
#include "bug.h"
#include "packetparser.h"
#include "global.h"

static void cleanup(void) {

    global_purge();
}

static void die_fatal(const char *trouble, const char *d, const char *fn) {

    cleanup();

    if (d) {
        if (fn) log_f5(trouble, " ", d, "/", fn);
        else log_f3(trouble, " ", d);
    }
    else {
        log_f1(trouble);
    }
    _exit(111);
}

static int _packet_kex_receive(void) {

    struct buf *b = &packet.kexrecv;
    long long pos = 0;
    crypto_uint8 ch;
    crypto_uint32 len;

    if (!packet_getall(b, SSH_MSG_KEXINIT)) return 0;

    /* parse packet */
    pos = packetparser_uint8(b->buf, b->len, pos, &ch);       /* SSH_MSG_KEXINIT */
    if (ch != SSH_MSG_KEXINIT) bug_proto();

    pos = packetparser_skip(b->buf, b->len, pos, 16);         /* cookie */

    pos = packetparser_uint32(b->buf, b->len, pos, &len);     /* kex algorithms */
    pos = packetparser_skip(b->buf, b->len, pos, len);
    log_i2("kex algorithms: ", (char *)b->buf + pos - len);

    pos = packetparser_uint32(b->buf, b->len, pos, &len);     /* server host key algorithms */
    pos = packetparser_skip(b->buf, b->len, pos, len);
    log_i2("server host key algorithms: ", (char *)b->buf + pos - len);

    pos = packetparser_uint32(b->buf, b->len, pos, &len);     /* encryption algorithms client to server */
    pos = packetparser_skip(b->buf, b->len, pos, len);
    log_i2("encryption algorithms client to server: ", (char *)b->buf + pos - len);

    pos = packetparser_uint32(b->buf, b->len, pos, &len);     /* encryption algorithms server to client */
    pos = packetparser_skip(b->buf, b->len, pos, len);
    log_i2("encryption algorithms server to client: ", (char *)b->buf + pos - len);

    pos = packetparser_uint32(b->buf, b->len, pos, &len);     /* mac algorithms client to server */
    pos = packetparser_skip(b->buf, b->len, pos, len);
    log_i2("mac algorithms client to server: ", (char *)b->buf + pos - len);

    pos = packetparser_uint32(b->buf, b->len, pos, &len);     /* mac algorithms server to client */
    pos = packetparser_skip(b->buf, b->len, pos, len);
    log_i2("mac algorithms server to client: ", (char *)b->buf + pos - len);

    pos = packetparser_uint32(b->buf, b->len, pos, &len);     /* compress algorithms client to server */
    pos = packetparser_skip(b->buf, b->len, pos, len);

    pos = packetparser_uint32(b->buf, b->len, pos, &len);     /* compress algorithms server to client */
    pos = packetparser_skip(b->buf, b->len, pos, len);

    pos = packetparser_uint32(b->buf, b->len, pos, &len);     /* languages client to server */
    pos = packetparser_skip(b->buf, b->len, pos, len);

    pos = packetparser_uint32(b->buf, b->len, pos, &len);     /* languages server to client */
    pos = packetparser_skip(b->buf, b->len, pos, len);

    pos = packetparser_uint8(b->buf, b->len, pos, &ch);       /* kex first packet follows */

    pos = packetparser_uint32(b->buf, b->len, pos, &len);     /* reserved */

    return 1;
}


int main(int argc, char **argv, char **envp) {

    pid_t pid;
    int tochild[2] = { -1, -1 };
    int fromchild[2] = { -1, -1 };

    if (argc < 2) _exit(111);
    if (!argv[0]) _exit(111);
    if (!argv[1]) _exit(111);
    ++argv;

    if (pipe(tochild) == -1) _exit(111);
    if (pipe(fromchild) == -1) _exit(111);
    pid = fork();
    if (pid == -1) _exit(111);

    if (pid == 0) {
        close(tochild[1]);
        close(fromchild[0]);
        close(2);
        if (dup2(tochild[0], 0) == -1) _exit(111);
        if (dup2(fromchild[1], 1) == -1) _exit(111);
        execvp(*argv, argv);
        _exit(111);
    }
    close(tochild[0]);
    close(fromchild[1]);

    close(0); if (dup2(fromchild[0], 0) == -1) _exit(111);
    close(1); if (dup2(tochild[1], 1) == -1) _exit(111);

    signal(SIGPIPE, SIG_IGN);

    global_init();

    log_init(2, "_tinysshd-printkex", 0, 0);

    if (!packet_hello_receive()) die_fatal("unable to receive hello-string", 0, 0);
    if (!packet_hello_send()) die_fatal("unable to send hello-string", 0, 0);
    if (!_packet_kex_receive()) die_fatal("unable to receive kex-message", 0, 0);

    _exit(111);
}
