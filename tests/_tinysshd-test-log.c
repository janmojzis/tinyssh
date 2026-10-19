/*
20261011
Public domain.

Writes log messages to standard error: a short one, one longer than
the log buffer, and a short one again. The shell test checks that every
message is one line and that long messages are truncated, not split.
*/

#include "log.h"

static char longstring[4096];

int main(void) {
    unsigned long long i;

    for (i = 0; i < sizeof longstring - 1; ++i) longstring[i] = 'x';
    longstring[sizeof longstring - 1] = 0;

    log_init(3, "tinysshd", 1, 0);
    log_d1("short message");
    log_d2("long message: ", longstring);
    log_d1("short message");
    return 0;
}
