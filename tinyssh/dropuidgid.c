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
*/
int dropuidgid(uid_t uid, gid_t gid) {

    if (uid != geteuid()) {
        if (setgroups(1, &gid) == -1) return 0;
        if (setgid(gid) == -1) return 0;
        if (setuid(uid) == -1) return 0;
        if (getgid() != gid) { errno = EPERM; return 0; }
        if (getuid() != uid) { errno = EPERM; return 0; }
    }
    return 1;
}
