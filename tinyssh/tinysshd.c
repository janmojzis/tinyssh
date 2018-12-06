/*
20181206
Jan Mojzis
Public domain.
*/

/*
Multi-call binary wrapper
*/

#include <unistd.h>
#include "str.h"
#include "main.h"

static char *basename(char *str) {

    char *s;
    char *ret = str;

    for (s = str; *s; ++s) {
        if (*s == '/') ret = s + 1;
    }
    return ret;
}

int main(int argc, char **argv) {

    char *x;
    long long xlen;

    if (argc < 1) _exit(100);
    if (!argv[0]) _exit(100);

    x = basename(argv[0]);
    if (!x) _exit(100);
    xlen = str_len(x);


    if (str_equaln(x, xlen, "tinysshd-printkey")) {
        return main_tinysshd_printkey(argc, argv);
    }
    else if (str_equaln(x, xlen, "tinysshd-makekey")){
        return main_tinysshd_makekey(argc, argv);
    }
    else {
        return main_tinysshd(argc, argv);
    }

    _exit(111);
}
