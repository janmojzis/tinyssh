#ifndef _RUN_H____
#define _RUN_H____

#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "fail.h"

static void _run_mustfail(char *file, unsigned long long line, void (*op)(void)) {      \
                                                                                        \
    pid_t pid;                                                                          \
    int status;                                                                         \
                                                                                        \
    pid = fork();                                                                       \
    if (pid == -1) fail_(file, line, "unable to fork");                                 \
                                                                                        \
    if (pid == 0) {                                                                     \
        close(2);                                                                       \
        op();                                                                           \
        _exit(0);                                                                       \
    }                                                                                   \
                                                                                        \
    while (waitpid(pid, &status, 0) != pid) {};                                         \
    if (!WIFEXITED(status)) fail_(file, line, "process killed");                        \
    if (!WEXITSTATUS(status)) fail_(file, line, "process exited with status = 0");      \
}

static void _run_mustpass(char *file, unsigned long long line, void (*op)(void)) {      \
                                                                                        \
    pid_t pid;                                                                          \
    int status;                                                                         \
                                                                                        \
    pid = fork();                                                                       \
    if (pid == -1) fail_(file, line, "unable to fork");                                 \
                                                                                        \
    if (pid == 0) {                                                                     \
        close(2);                                                                       \
        op();                                                                           \
        _exit(111);                                                                     \
    }                                                                                   \
                                                                                        \
    while (waitpid(pid, &status, 0) != pid) {};                                         \
    if (!WIFEXITED(status)) fail_(file, line, "process killed");                        \
    if (WEXITSTATUS(status)) fail_(file, line, "process exited with status != 0");      \
}

#define run_mustfail(x) _run_mustfail(__FILE__, __LINE__, (x));
#define run_mustpass(x) _run_mustpass(__FILE__, __LINE__, (x));

#endif
