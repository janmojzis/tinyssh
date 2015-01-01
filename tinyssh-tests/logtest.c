/*
20140319
Jan Mojzis
Public domain.
*/

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include "fail.h"
#include "byte.h"
#include "log.h"

static int readall(int fd, unsigned char *x, long long xlen) {

    long long r;
    long long len = 0;

    while (xlen > 0) {
        r = read(fd, x, xlen);
        if (r == 0) break;
        if (r <= 0) return -1;
        x += r;
        len += r;
        xlen -= r;
    }
    return len;
}

static unsigned char buf[1024];

#define LINE_EXP "?????????\n????????????????????? !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~?????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????"
static void run(void (*op)(void)) {

    pid_t pid;
    int status, fromchild[2];
    long long r, i, j;

    if (pipe(fromchild) == -1) fail("pipe() failure");
    pid = fork();
    if (pid == -1) fail("fork() failure");
    if (pid == 0) {
        close(fromchild[0]);
        if (dup2(fromchild[1], 2) == -1) _exit(111);
        op();
        _exit(0);
    }
    close(fromchild[1]);
    r = readall(fromchild[0], buf, sizeof buf);
    if (r == -1) fail("read() failure");

    j = 0;
    for (i = 0; i < r; ++i) {
        if (buf[i] == ' ') ++j;
        if (j == 3) break;
    }
    r -= i + 1;
    byte_copy(buf, r, buf + i + 1);

    for (r = r - 1; r > 0; --r) {
        if (buf[r] == ' ') { 
            buf[r] = 0;
            break;
        }
    }

    while (waitpid(pid, &status, 0) != pid) {};
    if (!WIFEXITED(status)) fail("process killed");
    if (WEXITSTATUS(status)) fail("process exited with status != 0");

    if (r < sizeof(LINE_EXP) - 1) fail("failure");
    if (!byte_isequal(buf, sizeof(LINE_EXP) - 1, LINE_EXP)) fail("failure");
}

static char line[256];

static void feed(void) {
    unsigned long long i;
    for (i = 0; i < 256; ++i) line[i] = (unsigned char)(i + 1);
}

static void test_fatal(void) {
    log_init(LOG_FATAL, "x", 1);
    log_d1("hidden");
    log_i1("hidden");
    log_w1("hidden");
    log_f1(line);
}

static void test_warning(void) {
    log_init(LOG_WARNING, "x", 1);
    log_d1("hidden");
    log_i1("hidden");
    log_w1(line);
}

static void test_info(void) {
    log_init(LOG_INFO, "x", 1);
    log_d1("hidden");
    log_i1(line);
}

static void test_debug(void) {
    log_init(LOG_DEBUG, "x", 1);
    log_d1(line);
}


int main(void) {

    feed();

    run(test_fatal);
    run(test_warning);
    run(test_info);
    run(test_debug);

    _exit(0);
}
