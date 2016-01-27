/*
20120923
Jan Mojzis
Public domain.
*/

#include "hasmlock.h"
#ifdef HASMLOCK
#include <sys/mman.h>
#endif
#include "trymlock.h"


/*
The 'trymlock' function tries to
lock the memory to make it unswappable.
*/
void trymlock(void *x, long long len) {
#ifdef HASMLOCK
    mlock(x, len);
#endif
}

/*
The 'trymunlock' function tries to
unlock the memory.
*/
void trymunlock(void *x, long long len) {
#ifdef HASMLOCK
    munlock(x, len);
#endif
}
