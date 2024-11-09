/*
20140214
Jan Mojzis
Public domain.

The 'global' library is used for global memory
initializition and global memory cleanup.
The 'global' library also has space
for 2 versatile buffers.
*/

#include <unistd.h>
#include "newenv.h"
#include "channel.h"
#include "packet.h"
#include "sshcrypto.h"
#include "purge.h"
#include "trymlock.h"
#include "global.h"

unsigned char global_bspace1[GLOBAL_BSIZE];
unsigned char global_bspace2[GLOBAL_BSIZE];

/*
Initialize memory space.
*/
void global_init(void) {

    packet_init();
    channel_init();
    newenv_init();

    sshcrypto_init();

    trymlock(global_bspace1, sizeof global_bspace1);
    trymlock(global_bspace2, sizeof global_bspace2);
    purge(global_bspace1, sizeof global_bspace1);
    purge(global_bspace2, sizeof global_bspace2);
}

/*
Remove sentitive data from allocated memory.
*/
void global_purge(void) {

    unsigned char stack[4096];

    purge(stack, sizeof stack);

    packet_purge();
    channel_purge();
    newenv_purge();

    sshcrypto_purge();

    purge(global_bspace1, sizeof global_bspace1);
    purge(global_bspace2, sizeof global_bspace2);
    trymunlock(global_bspace1, sizeof global_bspace1);
    trymunlock(global_bspace2, sizeof global_bspace2);
}

/*
Remove sentitive data from allocated memory
and exit with status 'x'.
*/
void global_die(int x) {
    global_purge();
    _exit(x);
}
