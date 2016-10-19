/*
20140420
Jan Mojzis
Public domain.
*/

#include <sys/types.h>
#include <unistd.h>
#include <grp.h>
#include "e.h"
#include "dropuidgid.h"

/*
The 'dropuidgid' function is used to drop root privileges.
If the process has appropriate permittions, the 'dropuidgid'
function sets user ID and group ID of calling process to
uid and gid.
The 'dropuidgid' function also initialize supplementary group IDs.
*/
int dropuidgid(const char *name, uid_t uid, gid_t gid) {

    /* we can change group only as the root */
    if (geteuid() == 0) {
        if (setgid(gid) == -1) return 0;
        if (getgid() != gid) { errno = EPERM; return 0; }
        if (name) if (initgroups(name, gid) == -1) return 0;
    }

    if (geteuid() != uid) {
        if (setuid(uid) == -1) return 0;
        if (getuid() != uid) { errno = EPERM; return 0; }
    }
    return 1;
}
