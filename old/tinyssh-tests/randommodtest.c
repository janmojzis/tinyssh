/*
20140416
Jan Mojzis
Public domain.
*/

#include <stdio.h>
#include <unistd.h>
#include "fail.h"
#include "randommod.h"

int main(void) {

    long long i, r;

    if (randommod(-1) != 0) fail("failure");
    if (randommod(0) != 0) fail("failure");
    if (randommod(1) != 0) fail("failure");
    for (i = 2; i < 10; ++i) {
        r = randommod(i);
        if (r > i - 1 || r < 0) fail("failure");
    }
    for (i = 18014398509481984ULL; i < 18014398509481994ULL; ++i) {
        if (randommod(i) == randommod(i)) fail("failure");
    }
    _exit(0);
}
