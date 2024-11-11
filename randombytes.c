#include "haslibrandombytes.h"
#ifndef HASLIBRANDOMBYTES
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "randombytes.h"

static int fd = -1;

void randombytes(void *xv, long long xlen) {

    long long i;
    unsigned char *x = xv;

    if (fd == -1) {
        for (;;) {
#ifdef O_CLOEXEC
            fd = open("/dev/urandom", O_RDONLY | O_CLOEXEC);
#else
            fd = open("/dev/urandom", O_RDONLY);
            fcntl(fd, F_SETFD, 1);
#endif
            if (fd != -1) break;
            sleep(1);
        }
    }

    while (xlen > 0) {
        if (xlen < 1048576)
            i = xlen;
        else
            i = 1048576;

        i = read(fd, x, i);
        if (i < 1) {
            sleep(1);
            continue;
        }

        x += i;
        xlen -= i;
    }
    __asm__ __volatile__("" : : "r"(xv) : "memory");
}
#else
__attribute__((unused)) static void _randombytes_dummy(void) {}
#endif
