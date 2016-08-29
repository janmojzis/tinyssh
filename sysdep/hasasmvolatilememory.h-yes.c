/* Public domain. */

void foo(void *yv, long long ylen);
void foo(void *yv, long long ylen) {
    char *y = yv;
    while (ylen > 0) { *y++ = 0; --ylen; }
    __asm__ __volatile__("" : : "r"(yv) : "memory");
}
