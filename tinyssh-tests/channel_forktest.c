/*
20140416
Jan Mojzis
Public domain.
*/

#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include "fail.h"
#include "writeall.h"
#include "readall.h"
#include "byte.h"
#include "channel.h"


/* test if channel_fork pipes works from child to parent */
static void test1(void) {

    int fd[3];
    pid_t pid;
    int status;
    char buf[5];

    pid = channel_fork(fd);
    if (pid == -1) fail("channel_fork failure");
    if (pid == 0) {
        if (writeall(1, "ahoj1", 5) == -1) fail("writeall failure");
        if (writeall(2, "ahoj2", 5) == -1) fail("writeall failure");
        _exit(0);
    }
    if (fd[0] == -1) fail("channel_fork failure");
    if (fd[1] == -1) fail("channel_fork failure");
    if (fd[2] == -1) fail("channel_fork failure");

    if (readall(fd[1], buf, 5) == -1) fail("readall failure");
    if (!byte_isequal(buf, 5, "ahoj1")) fail("readall failure");
    if (readall(fd[2], buf, 5) == -1) fail("readall failure");
    if (!byte_isequal(buf, 5, "ahoj2")) fail("readall failure");

    while (waitpid(pid, &status, 0) != pid) {};
    if (!WIFEXITED(status)) fail("process killed");
    if (WEXITSTATUS(status)) fail("process exited with status != 0");
}

/* test if channel_fork pipes works from parent to child */
static void test2(void) {

    int fd[3];
    pid_t pid;
    int status;
    char buf[5];

    pid = channel_fork(fd);
    if (pid == -1) fail("channel_fork failure");
    if (pid == 0) {
        if (readall(0, buf, 5) == -1) fail("readall failure");
        if (!byte_isequal(buf, 5, "ahoj0")) fail("readall failure");
        _exit(0);
    }
    if (fd[0] == -1) fail("channel_fork failure");
    if (fd[1] == -1) fail("channel_fork failure");
    if (fd[2] == -1) fail("channel_fork failure");

    if (writeall(fd[0], "ahoj0", 5) == -1) fail("writeall failure");

    while (waitpid(pid, &status, 0) != pid) {};
    if (!WIFEXITED(status)) fail("process killed");
    if (WEXITSTATUS(status)) fail("process exited with status != 0");
}

int main(void) {
    alarm(10);
    test1();
    test2();
    _exit(0);
}
