#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <unistd.h>
#include <stdlib.h>
#include "hasclosefrom.h"
#include "p.h"

/*
Close descriptors greater than or equal to lowfd, ignoring close errors.
The portable fallback only closes descriptors below 65536.
Negative lowfd is treated as 0.
*/
void p_closefrom(int lowfd) {

    if (lowfd < 0) lowfd = 0;

#ifdef HASCLOSEFROM
    /* Systems providing closefrom(). */
    (void) closefrom(lowfd);
#else
    /* Portable fallback for systems without closefrom(). */
    {
        long long i, maxfd = 4096;
#ifdef _SC_OPEN_MAX
        long long openmax = sysconf(_SC_OPEN_MAX);
        if (openmax > 65536) openmax = 65536;
        if (openmax > maxfd) maxfd = (int) openmax;
#endif
        for (i = (long long) lowfd; i < maxfd; ++i) close((int) i);
    }
#endif
}
