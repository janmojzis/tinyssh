#include <unistd.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "e.h"
#include "p.h"

static int isclosed(int fd) {
    errno = 0;
    return fcntl(fd, F_GETFD) == -1 && errno == EBADF;
}

static int check(int lowfd) {

    int fd, i, sparse[2], standard[3];
    static const int targets[2] = {5000, 65535};

    fd = open("/dev/null", O_RDONLY);
    if (fd == -1) return 111;
    if (dup2(fd, 16) != 16) return 111;
    if (dup2(fd, 17) != 17) return 111;
    for (i = 0; i < 2; ++i) {
        sparse[i] = fcntl(fd, F_DUPFD, targets[i]);
        if (sparse[i] == -1 && errno != EINVAL && errno != EMFILE) return 111;
    }
    for (i = 0; i < 3; ++i) standard[i] = fcntl(i, F_GETFD);

    p_closefrom(lowfd);
    for (i = 0; i < 3; ++i) {
        if (lowfd <= 0) {
            if (!isclosed(i)) return 111;
        }
        else if (fcntl(i, F_GETFD) != standard[i])
            return 111;
    }
    if (!isclosed(17)) return 111;
    if (lowfd > 16) {
        if (fcntl(16, F_GETFD) == -1) return 111;
    }
    else if (!isclosed(16))
        return 111;
    for (i = 0; i < 2; ++i) {
        if (sparse[i] != -1 && sparse[i] < 65536 && !isclosed(sparse[i]))
            return 111;
    }
    return 0;
}

int main(void) {

    int i, status;
    pid_t pid, r;
    static const int lowfds[5] = {INT_MIN, -1, 0, 3, 17};

    for (i = 0; i < 5; ++i) {
        pid = fork();
        if (pid == -1) return 111;
        if (pid == 0) _exit(check(lowfds[i]));
        do { r = waitpid(pid, &status, 0); } while (r == -1 && errno == EINTR);
        if (r != pid || !WIFEXITED(status) || WEXITSTATUS(status) != 0)
            return 111;
    }
    return 0;
}
