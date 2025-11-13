#include <sys/types.h>
#include <pwd.h>
#include <grp.h>

#include "static_compat.h"

#ifdef STATIC_BUILD

#include <errno.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <unistd.h>
#include <limits.h>

#include "str.h"
#include "getln.h"
#include "open.h"

#ifndef STATIC_COMPAT_PASSWD_BUFSIZE
#define STATIC_COMPAT_PASSWD_BUFSIZE 8192
#endif
#ifndef STATIC_COMPAT_NAME_LEN
#ifdef LOGIN_NAME_MAX
#define STATIC_COMPAT_NAME_LEN LOGIN_NAME_MAX
#else
#define STATIC_COMPAT_NAME_LEN 256
#endif
#endif
#ifndef STATIC_COMPAT_PATH_LEN
#ifdef PATH_MAX
#define STATIC_COMPAT_PATH_LEN PATH_MAX
#else
#define STATIC_COMPAT_PATH_LEN 4096
#endif
#endif
#ifndef STATIC_COMPAT_SHELL_LEN
#ifdef PATH_MAX
#define STATIC_COMPAT_SHELL_LEN PATH_MAX
#else
#define STATIC_COMPAT_SHELL_LEN 1024
#endif
#endif
#ifndef STATIC_COMPAT_GROUP_LINE_MAX
#define STATIC_COMPAT_GROUP_LINE_MAX 8192
#endif
#ifndef STATIC_COMPAT_MAX_GROUPS
#define STATIC_COMPAT_MAX_GROUPS 256
#endif

static struct passwd static_passwd;
static char static_passwd_name[STATIC_COMPAT_NAME_LEN];
static char static_passwd_dir[STATIC_COMPAT_PATH_LEN];
static char static_passwd_shell[STATIC_COMPAT_SHELL_LEN];
static const char static_empty_string[] = "";

static void static_compat_trim(char *line) {

    char *end = line;
    while (*end) ++end;
    while (end > line) {
        char ch = end[-1];
        if (ch == '\n' || ch == '\r') {
            end[-1] = 0;
            --end;
            continue;
        }
        break;
    }
}

static struct passwd *static_compat_search_passwd(const char *name, uid_t uid,
                                                  int by_name, int by_uid) {

    int fd = open_read("/etc/passwd");
    if (fd == -1) return 0;

    char line[STATIC_COMPAT_PASSWD_BUFSIZE];
    struct passwd *ret = 0;
    long long namelen = (by_name && name) ? str_len(name) : 0;
    int had_error = 0;
    int saved_errno = 0;

    int r;
    do {
        r = getln(fd, line, sizeof line);
        if (r == -1) {
            if (errno == ENOMEM) {
                fprintf(stderr,
                        "static_compat: /etc/passwd entry too long; "
                        "increase STATIC_COMPAT_PASSWD_BUFSIZE (%d) and recompile\n",
                        STATIC_COMPAT_PASSWD_BUFSIZE);
                exit(1);
            }
            had_error = 1;
            saved_errno = errno;
            ret = 0;
            break;
        }
        if (r == 0 && line[0] == 0) break;
        if (line[0] == '#' || line[0] == 0) continue;
        static_compat_trim(line);
        if (line[0] == 0) continue;

        char *fields[7];
        int idx = 0;
        char *p = line;
        fields[idx++] = p;
        while (*p && idx < 7) {
            if (*p == ':') {
                *p++ = 0;
                fields[idx++] = p;
            }
            else ++p;
        }
        if (idx < 7) continue;

        char *endptr = 0;
        unsigned long parsed_uid = strtoul(fields[2], &endptr, 10);
        if (endptr == fields[2]) continue;
        unsigned long parsed_gid = strtoul(fields[3], &endptr, 10);
        if (endptr == fields[3]) continue;

        long long fieldlen = str_len(fields[0]);
        int matched = 0;
        if (by_name && name && fieldlen == namelen &&
            str_equaln(fields[0], namelen, name))
            matched = 1;
        if (!matched && by_uid && (uid_t) parsed_uid == uid) matched = 1;
        if (!matched) continue;

        if (!str_copyn(static_passwd_name, sizeof static_passwd_name,
                        fields[0])) {
            fprintf(stderr,
                    "static_compat: passwd name exceeds STATIC_COMPAT_NAME_LEN "
                    "(%d); increase it and recompile\n",
                    STATIC_COMPAT_NAME_LEN);
            exit(1);
        }
        if (!str_copyn(static_passwd_dir, sizeof static_passwd_dir,
                        fields[5])) {
            fprintf(stderr,
                     "static_compat: passwd dir exceeds STATIC_COMPAT_PATH_LEN "
                     "(%d); increase it and recompile\n",
                     STATIC_COMPAT_PATH_LEN);
            exit(1);
        }
        if (!str_copyn(static_passwd_shell, sizeof static_passwd_shell,
                        fields[6])) {
            fprintf(stderr,
                    "static_compat: passwd shell exceeds STATIC_COMPAT_SHELL_LEN "
                    "(%d); increase it and recompile\n",
                    STATIC_COMPAT_SHELL_LEN);
            exit(1);
        }

        static_passwd.pw_name = static_passwd_name;
        static_passwd.pw_passwd = (char *) static_empty_string;
        static_passwd.pw_uid = (uid_t) parsed_uid;
        static_passwd.pw_gid = (gid_t) parsed_gid;
        static_passwd.pw_gecos = (char *) static_empty_string;
        static_passwd.pw_dir = static_passwd_dir;
        static_passwd.pw_shell = static_passwd_shell;

        ret = &static_passwd;
        errno = 0;
        break;
    } while (r > 0);

    close(fd);
    if (had_error) {
        errno = saved_errno;
        return 0;
    }
    if (!ret) errno = ENOENT;
    return ret;
}

static int static_compat_user_in_list(const char *list, const char *name) {

    if (!list || !name) return 0;
    long long needlen = str_len(name);
    const char *start = list;
    for (;;) {
        const char *end = start;
        while (*end && *end != ',') ++end;
        long long len = end - start;
        if (len == needlen && str_equaln(start, needlen, name)) return 1;
        if (!*end) break;
        start = end + 1;
    }
    return 0;
}

static int static_compat_add_group(gid_t *groups, int *count, gid_t gid) {

    int i;
    for (i = 0; i < *count; ++i)
        if (groups[i] == gid) return 0;
    if (*count >= STATIC_COMPAT_MAX_GROUPS) return -1;
    groups[(*count)++] = gid;
    return 1;
}

int static_compat_initgroups(const char *name, gid_t gid) {

    if (!name) {
        errno = EINVAL;
        return -1;
    }

    int fd = open_read("/etc/group");
    if (fd == -1) return -1;

    gid_t groups[STATIC_COMPAT_MAX_GROUPS];
    int ngroups = 0;
    if (static_compat_add_group(groups, &ngroups, gid) == -1) {
        fprintf(stderr,
                "static_compat: user belongs to too many groups (limit %d); "
                "increase STATIC_COMPAT_MAX_GROUPS and recompile\n",
                STATIC_COMPAT_MAX_GROUPS);
        exit(1);
    }

    char line[STATIC_COMPAT_GROUP_LINE_MAX];
    int r;
    do {
        r = getln(fd, line, sizeof line);
        if (r == -1) {
            if (errno == ENOMEM) {
                fprintf(stderr,
                        "static_compat: /etc/group entry too long; "
                        "increase STATIC_COMPAT_GROUP_LINE_MAX (%d) and recompile\n",
                        STATIC_COMPAT_GROUP_LINE_MAX);
                exit(1);
            }
            close(fd);
            return -1;
        }
        if (r == 0 && line[0] == 0) break;
        if (line[0] == '#' || line[0] == 0) continue;
        static_compat_trim(line);
        if (line[0] == 0) continue;

        char *fields[4];
        int idx = 0;
        char *p = line;
        fields[idx++] = p;
        while (*p && idx < 4) {
            if (*p == ':') {
                *p++ = 0;
                fields[idx++] = p;
            }
            else ++p;
        }
        if (idx < 4) continue;
        if (!static_compat_user_in_list(fields[3], name)) continue;

        char *endptr = 0;
        unsigned long parsed_gid = strtoul(fields[2], &endptr, 10);
        if (endptr == fields[2]) continue;
        int addres = static_compat_add_group(groups, &ngroups, (gid_t) parsed_gid);
        if (addres == -1) {
            fprintf(stderr,
                    "static_compat: user belongs to too many groups (limit %d); "
                    "increase STATIC_COMPAT_MAX_GROUPS and recompile\n",
                    STATIC_COMPAT_MAX_GROUPS);
            exit(1);
        }
    } while (r > 0);

    close(fd);
    if (setgroups(ngroups, groups) == -1) return -1;
    return 0;
}

struct passwd *static_compat_getpwnam(const char *name) {
    return static_compat_search_passwd(name, 0, 1, 0);
}

struct passwd *static_compat_getpwuid(uid_t uid) {
    return static_compat_search_passwd(0, uid, 0, 1);
}

#else

struct passwd *static_compat_getpwnam(const char *name) {
    return getpwnam(name);
}

struct passwd *static_compat_getpwuid(uid_t uid) {
    return getpwuid(uid);
}

int static_compat_initgroups(const char *name, gid_t gid) {
    return initgroups(name, gid);
}

#endif
