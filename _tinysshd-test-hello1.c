/*
20160225
Jan Mojzis
Public domain.
*/

#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

int main(int argc, char **argv) {

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
        close(1); /* simulate closed connection  */
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

    _exit(111);
}
