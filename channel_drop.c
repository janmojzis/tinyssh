/*
20140129
Jan Mojzis
Public domain.
*/

#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include "dropuidgid.h"
#include "newenv.h"
#include "channel.h"

int channel_droppriv(char *user, char **shell) {

    struct passwd *pw;
    char *name;

    pw = getpwnam(user);
    if (!pw) return 0;

    if (isatty(0)) {
        name = ttyname(0);
        if (!name) return 0;
        if (!newenv_env("SSH_TTY", name)) return 0;
        /* setowner */
        if (chown(name, pw->pw_uid, pw->pw_gid) == -1) return 0;
        if (chmod(name, 0600) == -1) return 0;
    }

    /* drop privileges */
    if (!dropuidgid(pw->pw_name, pw->pw_uid, pw->pw_gid)) return 0;

    if (chdir(pw->pw_dir) == -1) return 0;
    if (!newenv_env("HOME", pw->pw_dir)) return 0;
    if (!newenv_env("USER", pw->pw_name)) return 0;
    if (!newenv_env("LOGNAME", pw->pw_name)) return 0;
    if (!newenv_env("LOGIN", pw->pw_name)) return 0;
    if (!newenv_env("SHELL", pw->pw_shell)) return 0;

    *shell = pw->pw_shell;
    return 1;
}
