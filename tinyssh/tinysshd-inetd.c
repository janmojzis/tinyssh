/*
20140227
Jan Mojzis
Public domain.
*/

#include <sys/types.h>
#include <unistd.h>

int main(int argc, char **argv, char **envp) {

    char *logger[2] = { "logger", 0 };
    pid_t pid;
    int tochild[2] = { -1, -1 };

    if (argc < 2) _exit(111);
    if (!argv[0]) _exit(111);
    if (!argv[1]) _exit(111);
    ++argv;

    if (pipe(tochild) == -1) _exit(111);
    pid = fork();
    if (pid == -1) _exit(111);

    if (pid == 0) {
        close(tochild[1]);
        if (dup2(tochild[0], 0) == -1) _exit(111);
        execvp(*logger, logger);
        _exit(111);
    }
    close(tochild[0]);
    if (dup2(tochild[1], 2) == -1) _exit(111);

    execve(*argv, argv, envp);
    _exit(111);
}
