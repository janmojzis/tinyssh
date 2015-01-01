/*
20140212
Jan Mojzis
Public domain.

The 'logsys' library is used to login recording
and logout recording in the system using
utmp/utmpx ...

Portability - HELL !!
*/

/*
tested on:
FreeBSD 9.2
OpenBSD 5.3
NetBSD 6.1.2
Linux Debian 7.0 
SunOS 5.11
*/

#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <paths.h>

#include "hasutilh.h"
#ifdef HASUTILH
#include <util.h>
#endif

#include "hasutmpx.h"
#ifdef HASUTMPX
#include <utmpx.h>
#endif
#include "hasutmpxupdwtmpx.h"
#include "hasutmpxsyslen.h"

#include "hasutmp.h"
#ifdef HASUTMP
#include <utmp.h>
#endif
#include "hasutmpname.h"
#include "hasutmppid.h"
#include "hasutmptime.h"
#include "hasutmptv.h"
#include "hasutmptype.h"
#include "hasutmphost.h"
#include "hasutmpuser.h"
#include "hasutmplogwtmp.h"
#include "hasutmploginlogout.h"

#include "str.h"
#include "byte.h"
#include "logsys.h"

static void logsys_utmpx(const char *user, const char *host, const char *name, long long pid, int flaglogin) {

#ifdef HASUTMPX
    struct timeval tv;
    struct utmpx ut;

    byte_zero(&ut, sizeof ut);

    /* line */
    if (!name) name = ttyname(0);
    if (!name) return;
    if (str_start(name, "/dev/")) name += 5;
    str_copyn(ut.ut_line, sizeof ut.ut_line, name);

    /* host */
    str_copyn(ut.ut_host, sizeof ut.ut_host, host);
#ifdef HASUTMPXSYSLEN
    ut.ut_syslen = str_len(ut.ut_host) + 1;
#endif

    /* user */
    str_copyn(ut.ut_user, sizeof ut.ut_user, user);

    /* time */
    gettimeofday(&tv, 0);
    ut.ut_tv.tv_sec = tv.tv_sec;
    ut.ut_tv.tv_usec = tv.tv_usec;

    /* pid */
    if (pid == 0) pid = getpid();
    ut.ut_pid = pid;

    /* type */
    if (flaglogin) ut.ut_type = USER_PROCESS;
    else ut.ut_type = DEAD_PROCESS;

    /* update utmpx */
    setutxent();
    pututxline(&ut);
    endutxent();

    /* update wtmpx */
#if defined(_PATH_WTMPX) && defined(HASUTMPXUPDWTMPX)
    updwtmpx(_PATH_WTMPX, &ut);
#endif

#endif
}

static void logsys_utmp(const char *user, const char *host, const char *name, long long pid, int flaglogin) {

#ifdef HASUTMP
    struct timeval tv;
    struct utmp ut;

    byte_zero(&ut, sizeof ut);

    /* line */
    if (!name) name = ttyname(0);
    if (!name) return;
    if (str_start(name, "/dev/")) name += 5;
    str_copyn(ut.ut_line, sizeof ut.ut_line, name);

    /* host */
#ifdef HASUTMPHOST
    str_copyn(ut.ut_host, sizeof ut.ut_host, host);
#endif

    /* user */
#ifdef HASUTMPNAME
    str_copyn(ut.ut_name, sizeof ut.ut_name, user);
#endif
#ifdef HASUTMPUSER
    str_copyn(ut.ut_user, sizeof ut.ut_user, user);
#endif

    /* time */
    gettimeofday(&tv, 0);
#ifdef HASUTMPTIME
    ut.ut_time = tv.tv_sec;
#endif
#ifdef HASUTMPTV
    ut.ut_tv.tv_sec = tv.tv_sec;
    ut.ut_tv.tv_usec = tv.tv_usec;
#endif

    /* pid */
#ifdef HASUTMPPID
    if (pid == 0) pid = getpid();
    ut.ut_pid = pid;
#endif

    /* type */
#ifdef HASUTMPTYPE
    if (flaglogin) ut.ut_type = USER_PROCESS;
    else ut.ut_type = DEAD_PROCESS;
#endif
    if (flaglogin) {
#ifdef HASUTMPLOGINLOGOUT
        login(&ut);
#else
        setutent();
        pututline(&ut);
        endutent();
#endif
    }
    else {
#ifdef HASUTMPLOGINLOGOUT
        logout(ut.ut_line);
#else
        setutent();
        pututline(&ut);
        endutent();
#endif
#if defined(HASUTMPLOGWTMP)
        logwtmp(ut.ut_line, "", "");
#endif
    }

#endif
}

void logsys_login(const char *user, const char *host, const char *name, long long pid) {
    logsys_utmp(user, host, name, pid, 1);
    logsys_utmpx(user, host, name, pid, 1);
}
void logsys_logout(const char *user, const char *host, const char *name, long long pid) {
    logsys_utmp(user, host, name, pid, 0); 
    logsys_utmpx(user, host, name, pid, 0);
}
