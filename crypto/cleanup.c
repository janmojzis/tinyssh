#include "cleanup.h"
#include "hasasmvolatilememory.h"

void cleanup_(void *yv, long long ylen) {
    volatile char *y = (volatile char *)yv; 
    while (ylen > 0) { *y++ = 0; --ylen; }
#ifdef HASASMVOLATILEMEMORY
    __asm__ __volatile__("" : : "r"(yv) : "memory");
#endif
}
