#include "cleanup.h"

void cleanup_(void *yv, long long ylen) {
    volatile char *y = (volatile char *)yv; 
    while (ylen > 0) { *y++ = 0; --ylen; }
#ifdef __GNUC__
    __asm__ __volatile__("" : : "r"(yv) : "memory");
#endif
}
