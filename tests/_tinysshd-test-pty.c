#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "e.h"
#include "p.h"

static int transfer(int fd, char *buf, int len, int writing) {

    ssize_t r;
    int pos = 0;

    while (pos < len) {
        if (writing)
            r = write(fd, buf + pos, (size_t) (len - pos));
        else
            r = read(fd, buf + pos, (size_t) (len - pos));
        if (r == -1 && errno == EINTR) continue;
        if (r <= 0) return -1;
        pos += (int) r;
    }
    return 0;
}

static int child(int master, int slave, int target) {

    int fd, i;
    char input[2];

    alarm(5);
    close(master);
    if (target >= 0 && slave != target) {
        if (dup2(slave, target) != target) return 111;
        close(slave);
        slave = target;
    }
    if (p_login_tty(slave) == -1) return 111;
    if (getsid(0) != getpid()) return 111;
    for (i = 0; i < 3; ++i) {
        if (!isatty(i)) return 111;
    }
    if (slave > 2) {
        errno = 0;
        if (fcntl(slave, F_GETFD) != -1 || errno != EBADF) return 111;
    }
    fd = open("/dev/tty", O_RDWR);
    if (fd == -1) return 111;
    close(fd);
    if (transfer(0, input, sizeof input, 0) == -1) return 111;
    if (input[0] != 'x' || input[1] != '\n') return 111;
    if (write(1, "o", 1) != 1 || write(2, "k", 1) != 1) return 111;
    return 0;
}

static int check(int target) {

    int master, slave, status, ok;
    pid_t pid, r;
    char output[2], input[2] = {'x', '\n'};
    struct termios attrs;

    if (p_openpty(&master, &slave) == -1) return 111;
    if (!isatty(master) || !isatty(slave) || !ttyname(slave)) return 111;
    if (tcgetattr(slave, &attrs) == -1) return 111;
    attrs.c_lflag &= ~(tcflag_t) (ECHO | ECHONL);
    if (tcsetattr(slave, TCSANOW, &attrs) == -1) return 111;

    pid = fork();
    if (pid == -1) return 111;
    if (pid == 0) _exit(child(master, slave, target));
    close(slave);
    ok = transfer(master, input, sizeof input, 1) == 0 &&
         transfer(master, output, sizeof output, 0) == 0 && output[0] == 'o' &&
         output[1] == 'k';
    do { r = waitpid(pid, &status, 0); } while (r == -1 && errno == EINTR);
    close(master);
    if (!ok || r != pid || !WIFEXITED(status) || WEXITSTATUS(status) != 0)
        return 111;
    return 0;
}

int main(void) {

    int i;
    static const int targets[4] = {-1, 0, 1, 2};

    alarm(20);
    for (i = 0; i < 4; ++i) {
        if (check(targets[i]) != 0) return 111;
    }
    return 0;
}
