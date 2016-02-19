#include <sys/types.h>
#include <unistd.h>
#include "open.h"
#include "load.h"
#include "writeall.h"


static const char base32chars[] = "0123456789bcdfghjklmnpqrstuvwxyz";

static int base32encode(unsigned char *out, long long outlen, const unsigned char *in, long long inlen) {

    long long i = 0, j = 0;
    unsigned long long v = 0, bits = 0;

    while (j < inlen) {
        v |= in[j++] << bits;
        bits += 8;

        while (bits >= 5) {
            if (i >= outlen) return 0;
            out[i++] = base32chars[v & 31];
            bits -= 5; v >>= 5;
        }
    }
    if (bits) {
        if (i >= outlen) return 0;
        out[i++] = base32chars[v & 31];
        bits -= 5; v >>= 5;
    }
    return 1;
}

static unsigned char pk[32];
static unsigned char out[((32 * 8) + 4) / 5];

int main(int argc, char **argv) {

    if (!argv[0]) _exit(100);
    if (!argv[1]) _exit(100);

    if (chdir(argv[1]) == -1) _exit(111);
    if (load("publickey", pk, sizeof pk) == -1) _exit(111);

    if (!base32encode(out, sizeof out, pk, sizeof pk)) _exit(111);
    out[sizeof out - 1] = '\n';

    if (writeall(1, out, sizeof out) == -1) _exit(111);
    _exit(0);
}
