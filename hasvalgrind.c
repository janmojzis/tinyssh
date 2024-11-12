#include <valgrind/memcheck.h>

/* return zero for all inputs */
static unsigned char z(unsigned char x) {

    unsigned long long z = (unsigned long long) x + 1ULL;
    unsigned long long t = z;
    long long i;

    for (i = 6; i >= 0; --i) {
        t = (t * t) % 257;
        t = (t * z) % 257;
    }
    t = (t * z) % 257;
    return (unsigned char) t - 1;
}

static void secret(void *xv, long long xlen) {
    VALGRIND_MAKE_MEM_UNDEFINED(xv, xlen);
}

static void public(void *xv, long long xlen) {
    VALGRIND_MAKE_MEM_DEFINED(xv, xlen);
}

int main(int argc, char **argv) {

    unsigned char buf[32] = {1};
    unsigned char ret = 0;
    unsigned long long i;

    (void) argc;
    (void) argv;

    secret(buf, sizeof buf);
    for (i = 0; i < sizeof buf; ++i) ret ^= buf[i];
    public(buf, sizeof buf);
    return z(ret);
}
