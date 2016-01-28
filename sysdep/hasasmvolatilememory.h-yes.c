/* Public domain. */
#include <stdio.h>

static void testcleanup(void *yv, long long ylen) {
    char *y = yv;
    while (ylen > 0) { *y++ = 0; --ylen; }
    __asm__ __volatile__("" : : "r"(yv) : "memory");
}

int main(void) {

    char buf[7] = "abcdef";
    char ret = 0;
    long long i;

    testcleanup(buf, sizeof buf);
    for (i = 0; i < sizeof buf; ++i) ret |= buf[i];
    if (ret) return 1;

    printf("#define HASASMVOLATILEMEMORY 1\n");
    return 0;
}
