#include <lib1305.h>

int main(int argc, char **argv) {
    int ret = 0;
    long long i;
    unsigned char k[poly1305_KEYBYTES] = {2};
    unsigned char m[16] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                           0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    unsigned char a[poly1305_BYTES];
    unsigned char o[poly1305_BYTES] = {3};

    (void) argc;
    (void) argv;

    ret |= -poly1305_verify(0, m, sizeof m, k);
    poly1305(a, m, sizeof m, k);
    for (i = 0; i < poly1305_BYTES; ++i) ret |= (o[i] ^ a[i]);
    return ret;
}
