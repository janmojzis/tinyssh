/*
20140129
Jan Mojzis
Public domain.
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include "dropuidgid.h"
#include "newenv.h"
#include "channel.h"

static int setowner(char *name, struct passwd *pw) {

    struct group *grp;
    gid_t gid;
    mode_t mode;
    struct stat st;

    grp = getgrnam("tty");
    if (grp) {
        gid = grp->gr_gid;
        mode = S_IRUSR | S_IWUSR | S_IWGRP;
    }
    else {
        gid = pw->pw_gid;
        mode = S_IRUSR | S_IWUSR | S_IWGRP | S_IWOTH;
    }

    if (stat(name, &st) == -1) return 0;
    if (chown(name, pw->pw_uid, gid) == -1) return 0;

    if ((st.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO)) != mode) {
        if (chmod(name, mode) == -1) return 0;
    }

    return 1;
}


int channel_droppriv(char *user, char **shell) {

    struct passwd *pw;
    char *name;

    pw = getpwnam(user);
    if (!pw) return 0;

    if (isatty(0)) {
        name = ttyname(0);
        if (!name) return 0;
        if (!newenv_env("SSH_TTY", name)) return 0;
        if (!setowner(name, pw)) return 0;
    }

    /* drop privileges */
    if (!dropuidgid(pw->pw_uid, pw->pw_gid)) return 0;

    if (chdir(pw->pw_dir) == -1) return 0;
    if (!newenv_env("HOME", pw->pw_dir)) return 0;
    if (!newenv_env("USER", pw->pw_name)) return 0;
    if (!newenv_env("LOGNAME", pw->pw_name)) return 0;
    if (!newenv_env("LOGIN", pw->pw_name)) return 0;
    if (!newenv_env("SHELL", pw->pw_shell)) return 0;

    *shell = pw->pw_shell;
    return 1;
}
