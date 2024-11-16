#include <randombytes.h>

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

int main(int argc, char **argv) {
    unsigned char buf[32], ret = 0;
    const char *source;
    unsigned long long i;

    (void) argc;
    (void) argv;

    randombytes(buf, sizeof buf);
    for (i = 0; i < sizeof buf; ++i) ret |= z(buf[i]);
    source = randombytes_source();
    for (i = 0; source[i]; ++i) ret |= z((unsigned char) source[i]);
    return z(ret);
}
