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

static int logflagfnln = 1;
static int logflagsyslog = 0;

static char chars[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRTSUVWXYZ0123456789";

void log_init(int level, const char *text, int line, int flagsyslog) {

    long long i;

    loglevel = level;
    logtext  = text;
    logflagfnln  = line;

    for (i = 0; i < sizeof logstring; ++i) {
        logstring[i] = chars[randommod(sizeof chars - 1)];
    }
    logstring[sizeof logstring - 1] = 0;

    if (flagsyslog) {
        openlog(text, 0, LOG_DAEMON);
        logflagsyslog = 1;
    }

    errno = 0;
}


char *log_string(void) {
    return logstring;
}

static char buf[257];
static long long buflen = 0;

static void flush(void) {

    if (logflagsyslog) {
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
    char b[NUMTOSTR_LEN];
    outs(numtostr(b, n));
}

void log_9_(
    int level
    ,int ignoreerrno
    ,const char *f
    ,unsigned long long l
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
    const char *m;

    if (level > loglevel) return;

    s[0] = s0; s[1] = s1; s[2] = s2; s[3] = s3;
    s[4] = s4; s[5] = s5; s[6] = s6; s[7] = s7;
    s[8] = s8;

    switch (level) {
        case -1:
            m = "BUG";
            break;
        case 0:
            m = "usage";
            break;
        case  1:
            m = "fatal";
            break;
        case  2: 
            if (!ignoreerrno) m = "warning";
            else m = "info";
            break;
        case  3:
            m = "debug";
            break;
        default:
            m = "unknown";
            break;
    }


    /* name: session: level: message (error){file:line} */

    /* 'name:' */
    do {
        if (level == 0)   break; /* don't print in usage level */
        if (logflagsyslog) break; /* don't print in syslog mode */
        outs(logtext); outs(": ");
    } while (0);

    /* 'session:' */
    do {
        if (level == 0) break;  /* don't print in usage level */
        if (!logflagfnln) break; /* don't print when disabled   */
        outs(logstring); outs(": ");
    } while (0);

    /* 'level:' */
    do {
        if (level == 0) break; /* don't print in usage level */
        outs(m); outs(": ");
    } while (0);

    /* 'message' */
    for (i = 0; i < 9 && s[i]; ++i) outs(s[i]);
    outs(" ");

    /* '(error)' */
    do {
        if (!errno)      break; /* don't print when errno = 0 */
        if (ignoreerrno) break; /* don't print when disabled   */
        outs("("); outs(e_str(errno)); outs(")");
    } while (0);

    /* {file:line} */
    do {
        if (level == 0) break;  /* don't print in usage level */
        if (!f)          break;  /* don't print when no f      */
        if (!l)          break;  /* don't print when no l      */
        if (!logflagfnln && level != -1) break; /* don't print when disabled   */
        outs("{"); outs(f); outs(":"); outnum(l); outs("}");
    } while (0);

    outs("\n");
    flush();
    return;
}
