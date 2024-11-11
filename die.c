#include "global.h"
#include "log.h"
#include "die.h"

void die_usage(const char *x) {

    log_u1(x);
    global_die(100);
}

void die_fatal_(const char *fn, unsigned long long line, const char *trouble, const char *d, const char *f) {

    if (d) {
        if (f) log_9_(1, 0, fn, line, trouble, " ", d, "/", f, 0, 0, 0, 0);
        else log_9_(1, 0, fn, line, trouble, " ", d, 0, 0, 0, 0, 0, 0);
    }
    else {
        log_9_(1, 0, fn, line, trouble, 0, 0, 0, 0, 0, 0, 0, 0);
    }
    global_die(111);
}
