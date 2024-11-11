/*
20140304
Jan Mojzis
Public domain.
*/

#include <unistd.h>
#include "global.h"

#ifndef GLOBAL_BSIZE
error!
#endif

int main(void) {
    global_init();
    global_init();
    global_purge();
    global_purge();
    global_init();
    global_init();
    _exit(0);
}
