/*
20140319
Jan Mojzis
Public domain.
*/

#include <signal.h>
#include <unistd.h>
#include <string.h>
#include "fail.h"
#include "ssh.h"

/*
grep SSH_ * | sed 's/.*SSH_/SSH_/g' | cut -d ',' -f1 | cut -d ' ' -f1 | cut -d ')' -f1 | cut -d ':' -f1 | cut -d '"' -f1 | cut -d ';' -f1 | sort -u | grep -v 'SSH_TTY\|SSH_CONNECTION'| while read x; do echo \#ifndef $x; echo error! ;echo \#endif; done
*/

#ifndef SSH_MSG_CHANNEL_CLOSE
error!
#endif
#ifndef SSH_MSG_CHANNEL_DATA
error!
#endif
#ifndef SSH_MSG_CHANNEL_EOF
error!
#endif
#ifndef SSH_MSG_CHANNEL_EXTENDED_DATA
error!
#endif
#ifndef SSH_MSG_CHANNEL_OPEN
error!
#endif
#ifndef SSH_MSG_CHANNEL_OPEN_CONFIRMATION
error!
#endif
#ifndef SSH_MSG_CHANNEL_OPEN_FAILURE
error!
#endif
#ifndef SSH_MSG_CHANNEL_REQUEST
error!
#endif
#ifndef SSH_MSG_CHANNEL_SUCCESS
error!
#endif
#ifndef SSH_MSG_CHANNEL_WINDOW_ADJUST
error!
#endif
#ifndef SSH_MSG_DEBUG
error!
#endif
#ifndef SSH_MSG_DISCONNECT
error!
#endif
#ifndef SSH_MSG_IGNORE
error!
#endif
#ifndef SSH_MSG_KEXDH_INIT
error!
#endif
#ifndef SSH_MSG_KEXDH_REPLY
error!
#endif
#ifndef SSH_MSG_KEXINIT
error!
#endif
#ifndef SSH_MSG_NEWKEYS
error!
#endif
#ifndef SSH_MSG_SERVICE_ACCEPT
error!
#endif
#ifndef SSH_MSG_SERVICE_REQUEST
error!
#endif
#ifndef SSH_MSG_UNIMPLEMENTED
error!
#endif
#ifndef SSH_MSG_USERAUTH_FAILURE
error!
#endif
#ifndef SSH_MSG_USERAUTH_PK_OK
error!
#endif
#ifndef SSH_MSG_USERAUTH_REQUEST
error!
#endif
#ifndef SSH_MSG_USERAUTH_SUCCESS
error!
#endif
#ifndef SSH_OPEN_ADMINISTRATIVELY_PROHIBITED
error!
#endif

int main(void) {

    if (strcmp(ssh_sigstr(SIGTERM), "TERM")) fail("failure")
    if (strcmp(ssh_sigstrlong(SIGTERM), "SIGTERM (termination)")) fail("failure")
    if (strcmp(ssh_sigstr(-1), "UNKNOWN")) fail("failure")
    if (strcmp(ssh_sigstrlong(-1), "UNKNOWN SIGNAL")) fail("failure")
    _exit(0);
}
