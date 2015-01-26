#include "global.h"
#include "log.h"
#include "die.h"

void die_usage(const char *x) {

    log_u1(x);
    global_die(100);
}

void die_fatal(const char *trouble, const char *d, const char *fn) {

    if (d) {
        if (fn) log_f5(trouble, " ", d, "/", fn);
        else log_f3(trouble, " ", d);
    }
    else {
        log_f1(trouble);
    }
    global_die(111);
}
