/*
20140202
Jan Mojzis
Public domain.

The 'log' library is used to write log messages
on standard error output including source file,
function and line number.
Non-printable characters are replaced using '?'.
The 'log' library also supports syslog.
*/

#include <syslog.h>
#include "writeall.h"
#include "randommod.h"
#include "purge.h"
#include "numtostr.h"
#include "e.h"
#include "log.h"

static const char *logtext = "x";
static char logstring[9] = "________";
static int loglevel = 1;
static int logline = 1;
static int logsyslog = 0;

void log_init(int level, const char *text, int line, int flagsyslog) {

    long long i;
    static char chars[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRTSUVWXYZ0123456789";

    loglevel = level;
    logtext  = text;
    logline  = line;

    for (i = 0; i < sizeof logstring; ++i) {
        logstring[i] = chars[randommod(sizeof chars - 1)];
    }
    logstring[sizeof logstring - 1] = 0;

    if (flagsyslog) {
        openlog(text, 0, LOG_DAEMON);
        logsyslog = 1;
    }

    errno = 0;
}

char *log_string(void) {
    return logstring;
}

static char buf[257];
static long long buflen = 0;

static void flush(void) {

    if (logsyslog) {
        buf[buflen] = 0;
        syslog(LOG_INFO, "%s", buf);
    }
    else {
        writeall(2, buf, buflen);
    }
    buflen = 0;
    purge(buf, buflen);
}

static void outs(const char *x) {

    long long i;

    for(i = 0; x[i]; ++i) {
        if (buflen >= sizeof buf - 1) flush();
        if (x[i] == '\n')  buf[buflen++] = '\n';
        else if (x[i] < 32)  buf[buflen++] = '?';
        else if (x[i] > 126) buf[buflen++] = '?';
        else buf[buflen++] = x[i];
    }
}

static void outnum(unsigned long long n) {
    outs(numtostr(0, n));
}

void log_9_(
    int level
    ,const char *f
    ,unsigned long long l
    ,const char *m
    ,const char *s0
    ,const char *s1
    ,const char *s2
    ,const char *s3
    ,const char *s4
    ,const char *s5
    ,const char *s6
    ,const char *s7
    ,const char *s8
)
{
    const char *s[9];
    long long i;

    if (level > loglevel) return;

    s[0] = s0; s[1] = s1; s[2] = s2; s[3] = s3;
    s[4] = s4; s[5] = s5; s[6] = s6; s[7] = s7;
    s[8] = s8;

    outs(logtext); outs(": ");
    if (logline) { outs(logstring); outs(": "); }
    outs(m); outs(": ");

    for (i = 0; i < 9 && s[i]; ++i) {
        outs(s[i]);
    }

    outs(" ");
    if (level != 2 /*LOG_INFO*/ && errno) { outs("("); outs(e_str(errno)); outs(")"); }
    if (f && l && logline) { outs("{"); outs(f); outs(":"); outnum(l); outs("}"); }
    outs("\n");
    flush();
    return;
}
