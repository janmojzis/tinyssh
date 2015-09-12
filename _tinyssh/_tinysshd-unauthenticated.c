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

static int _packet_debug(struct buf *b) {
    buf_purge(b);
    buf_putnum8(b, SSH_MSG_DEBUG);              /* SSH_MSG_DEBUG  */
    buf_putnum8(b, 1);                          /* always_display */
    buf_putstring(b, "test message");           /* message in ISO-10646 UTF-8 encoding [RFC3629] */
    buf_putstring(b, "");                       /* language tag [RFC3066] */
    packet_put(b);
    return packet_sendall();
}


static unsigned char bspace[1024];
static struct buf b;


int main(int argc, char **argv) {

    pid_t pid;
    int tochild[2] = { -1, -1 };
    int fromchild[2] = { -1, -1 };
    long long i;

    if (argc < 2) _exit(111);
    if (!argv[0]) _exit(111);
    if (!argv[1]) _exit(111);
    ++argv;

    buf_init(&b, bspace, sizeof bspace);

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

    log_init(1, "_tinysshd-printkex", 0, 0);

    if (!packet_hello_receive()) die_fatal("unable to receive hello-string", 0, 0);
    if (!packet_hello_send()) die_fatal("unable to send hello-string", 0, 0);

    for (i = 1; i < 10 * PACKET_UNAUTHENTICATED_MESSAGES; ++i) {
        if (!_packet_debug(&b)) {
            if (i < PACKET_UNAUTHENTICATED_MESSAGES) die_fatal("unable to send debug message - parent died too early", 0, 0);
            _exit(0); /* limitation works */
        }
    }

    die_fatal("limitation is not working !!!!!!!!!!", 0, 0);
    _exit(111);
}
