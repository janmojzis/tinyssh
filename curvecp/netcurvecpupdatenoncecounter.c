/*
20121211
Jan Mojzis
Public domain.
*/

#include <unistd.h>
#include "open.h"
#include "nanoseconds.h"
#include "byte.h"
#include "uint64_pack.h"
#include "savesync.h"
#include "die.h"
#include "e.h"
#include "load.h"

#define FATAL "FATAL: fatal: "

void die_usage(void) {
    die_1(100, "netcurvecpupdatenoncecounter: usage: netcurvecpupdatenoncecounter keydir\n");
}

void die_fatal(const char *trouble, const char *fn) {

    if (errno) {
        if (fn) die_7(111, FATAL, trouble, " ", fn, ": ", e_str(errno), "\n");
        die_5(111, FATAL, trouble, ": ", e_str(errno), "\n");
    }
    if (fn) die_5(111, FATAL, trouble, " ", fn, "\n");
    die_3(111, FATAL, trouble, "\n");
}


unsigned char n[8];

int main(int argc, char **argv) {

    char *d, *ns = 0, *x;
    unsigned char noncemask = 0;
    unsigned char noncedata = 0;
    long long i;
    int flagtime = 0;

    if (!argv[0]) die_usage();
    for (;;) {
        if (!argv[1]) break;
        if (argv[1][0] != '-') break;
        x = *++argv;
        if (x[0] == '-' && x[1] == 0) break;
        if (x[0] == '-' && x[1] == '-' && x[2] == 0) break;
        while (*++x) {
            if (*x == 't') { flagtime = 1; continue; }
            if (*x == 's') {
                if (x[1]) { ns = x + 1; break; }
                if (argv[1]) { ns = *++argv; break; }
            }
            die_usage();
        }
    }
    if (!*++argv) die_usage();
    d = *argv;

    if (chdir(d) == -1) die_fatal("unable to chdir to directory", d);
    if (open_lock(".expertsonly/lock") == -1) die_fatal("unable to lock directory", d);

    uint64_pack(n, nanoseconds());
    if (!flagtime) {
        if (load(".expertsonly/noncecounter", n, sizeof n) == -1) die_fatal("unable to load noncecounter from directory", d);
    }

    if (ns) {
        i = 0;
        while (i < 8) {
            if (!ns[i]) break;
            if (ns[i] != '0' && ns[i] != '1') break;

            noncemask >>= 1; 
            noncedata >>= 1;
            noncedata += (ns[i] - '0') * 0x80;
            ++i;
        }
        while(i < 8) {
            noncemask >>= 1;
            noncedata >>= 1;
            noncemask += 0x80;
            ++i;
        }
        n[7] &= noncemask;
        n[7] += noncedata;
    }

    if (savesync(".expertsonly/noncecounter", n, sizeof n) == -1) die_fatal("unable to update noncecounter in directory", d);
    _exit(0);
}
