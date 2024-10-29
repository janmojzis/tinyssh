#ifndef _FAIL_H____
#define _FAIL_H____

#include <stdio.h>
#include <unistd.h>
#include "fail.h"

#define fail_(a, b, x) do { fprintf(stderr, "%s:%llu: %s\n", (a), (unsigned long long)(b), (x)); fflush(stderr); _exit(111); } while (0);
#define fail(x) fail_(__FILE__, __LINE__, (x))


static void fail_printdata(const char *text, const unsigned char *data, long long len) {

    long long i;

    fprintf(stderr, "%s = {\n    ", text);
    for (i = 0; i < len; ++i) {
        if (i == len - 1) {
            fprintf(stderr, "0x%02x", data[i]);
        }
        else {
            fprintf(stderr, "0x%02x, ", data[i]);
            if (i % 8 == 7) fprintf(stderr, "\n    ");
        }
    }
    fprintf(stderr, "\n};\n");
    fflush(stderr);
}

#endif
