/*
20140302
Jan Mojzis
Public domain.
*/

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include "fail.h"
#include "coe.h"

int main(void) {

    int pi[2];
    long long i;

    if (pipe(pi) == -1) fail("failure");
    for (i = 0; i < 2; ++i) {
        if (fcntl(pi[i], F_GETFD) != 0) fail("failure");
        coe_enable(pi[i]);
        if (fcntl(pi[i], F_GETFD) == 0) fail("failure");
        coe_disable(pi[i]);
        if (fcntl(pi[i], F_GETFD) != 0) fail("failure");
    }
    _exit(0);
}

