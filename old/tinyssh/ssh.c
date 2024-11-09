#include <signal.h>
#include "ssh.h"

#define X(e,s) if (i == e) return s;

/*
Return short name of signal.
*/
const char *ssh_sigstr(int i) {

#ifdef SIGABRT
    X(SIGABRT, "ABRT");
#endif
#ifdef SIGALRM
    X(SIGALRM, "ALRM");
#endif
#ifdef SIGFPE
    X(SIGFPE, "FPE");
#endif
#ifdef SIGHUP
    X(SIGHUP, "HUP");
#endif
#ifdef SIGILL
    X(SIGILL, "ILL");
#endif
#ifdef SIGINT
    X(SIGINT, "INT");
#endif
#ifdef SIGKILL
    X(SIGKILL, "KILL");
#endif
#ifdef SIGPIPE
    X(SIGPIPE, "PIPE");
#endif
#ifdef SIGQUIT
    X(SIGQUIT, "QUIT");
#endif
#ifdef SIGSEGV
    X(SIGSEGV, "SEGV");
#endif
#ifdef SIGTERM
    X(SIGTERM, "TERM");
#endif
#ifdef SIGUSR1
    X(SIGUSR1, "USR1");
#endif
#ifdef SIGUSR2
    X(SIGUSR2, "USR2");
#endif

    return "UNKNOWN";
}

/*
Return long name of signal.
*/
const char *ssh_sigstrlong(int i) {

#ifdef SIGABRT
    X(SIGABRT, "SIGABRT (abort)")
#endif
#ifdef SIGALRM
    X(SIGALRM, "SIGALRM (alarm clock)")
#endif
#ifdef SIGFPE
    X(SIGFPE, "SIGFPE (floating-point exception)")
#endif
#ifdef SIGILL
    X(SIGILL, "SIGILL (illegal instruction)")
#endif
#ifdef SIGINT
    X(SIGINT, "SIGINT (interrupt)")
#endif
#ifdef SIGKILL
    X(SIGKILL, "SIGKILL (kill, unblockable)")
#endif
#ifdef SIGPIPE
    X(SIGPIPE, "SIGPIPE (broken pipe)")
#endif
#ifdef SIGQUIT 
    X(SIGQUIT, "SIGQUIT (quit)")
#endif
#ifdef SIGSEGV
    X(SIGSEGV, "SIGSEGV (segment violation)")
#endif
#ifdef SIGTERM
    X(SIGTERM, "SIGTERM (termination)")
#endif
#ifdef SIGUSR1
    X(SIGUSR1, "SIGUSR1 (user defined signal 1)")
#endif
#ifdef SIGUSR2
    X(SIGUSR2, "SIGUSR2 (user defined signal 2)")
#endif

    return "UNKNOWN SIGNAL";
}

