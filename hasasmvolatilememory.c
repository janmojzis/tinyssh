/* Public domain. */

static void foo(void *yv, long long ylen) {
    char *y = yv;
    while (ylen > 0) { *y++ = 0; --ylen; }
    __asm__ __volatile__("" : : "r"(yv) : "memory");
}

int main(void) {
    char x[16];
    foo(x, sizeof x);
    return 0;
}
