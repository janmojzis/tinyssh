/*
20260906
Public domain.
*/

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "e.h"
#include "getln.h"
#include "log.h"

extern int subprocess_auth_checkpath_(char *, long long, uid_t, int);
extern int subprocess_auth_authorizedkeys_(const char *, const char *,
                                           const char *, char *, long long);

static char path[4096];
static char buf[4096];

static int save(const void *data, size_t len, mode_t mode) {
    const char *x = data;
    size_t pos = 0;
    ssize_t r;
    int fd;

    fd = open("authorized_keys", O_WRONLY | O_CREAT | O_TRUNC, mode);
    if (fd == -1) return 0;
    if (fchmod(fd, mode) == -1) {
        close(fd);
        return 0;
    }
    while (pos < len) {
        r = write(fd, x + pos, len - pos);
        if (r <= 0) {
            close(fd);
            return 0;
        }
        pos += (size_t) r;
    }
    return close(fd) == 0;
}

static int authorized(void) {
    return subprocess_auth_authorizedkeys_("ssh-ed25519", "testkey", ".",
                                           buf, sizeof buf);
}

static int descriptor_identity(void) {
    static const char safe[] = "ssh-ed25519 otherkey\n";
    static const char unsafe[] = "ssh-ed25519 testkey\n";
    int fd;
    int ok;

    if (!save(safe, sizeof safe - 1, 0600)) return 0;
    fd = open("authorized_keys", O_RDONLY | O_NONBLOCK);
    if (fd == -1) return 0;
    if (unlink("authorized_keys") == -1) return 0;
    if (!save(unsafe, sizeof unsafe - 1, 0600)) return 0;
    ok = !subprocess_auth_checkpath_(path, sizeof path, geteuid(), fd);
    close(fd);
    if (unlink("authorized_keys") == -1) return 0;
    return ok;
}

static int unsafe_file(void) {
    static const char contents[] = "ssh-ed25519 testkey\n";

    if (!save(contents, sizeof contents - 1, 0666)) return 0;
    if (authorized()) return 0;
    return unlink("authorized_keys") == 0;
}

static int fifo(void) {
    int fd;
    int ok;

    if (mkfifo("authorized_keys", 0600) == -1) return 0;
    fd = open("authorized_keys", O_RDWR | O_NONBLOCK);
    if (fd == -1) return 0;
    alarm(2);
    ok = !authorized();
    alarm(0);
    close(fd);
    if (unlink("authorized_keys") == -1) return 0;
    return ok;
}

static int safe_symlink(void) {
    static const char contents[] = "ssh-ed25519 testkey\n";
    int ok;

    if (mkdir("safe", 0700) == -1 || chdir("safe") == -1) return 0;
    if (!save(contents, sizeof contents - 1, 0600)) return 0;
    if (rename("authorized_keys", "target") == -1 || chdir("..") == -1)
        return 0;
    if (symlink("safe/target", "authorized_keys") == -1) return 0;
    ok = authorized();
    if (unlink("authorized_keys") == -1 || unlink("safe/target") == -1 ||
        rmdir("safe") == -1)
        return 0;
    return ok;
}

static int unsafe_symlink(void) {
    static const char contents[] = "ssh-ed25519 testkey\n";
    int ok;

    if (mkdir("unsafe", 0777) == -1 || chmod("unsafe", 0777) == -1 ||
        chdir("unsafe") == -1)
        return 0;
    if (!save(contents, sizeof contents - 1, 0600)) return 0;
    if (rename("authorized_keys", "target") == -1 || chdir("..") == -1)
        return 0;
    if (symlink("unsafe/target", "authorized_keys") == -1) return 0;
    ok = !authorized();
    if (unlink("authorized_keys") == -1 || unlink("unsafe/target") == -1 ||
        rmdir("unsafe") == -1)
        return 0;
    return ok;
}

static int nul_before_key(void) {
    static const char contents[] =
        "# comment\0ignored\nssh-ed25519 testkey\n";
    int ok;

    if (!save(contents, sizeof contents - 1, 0600)) return 0;
    ok = authorized();
    if (unlink("authorized_keys") == -1) return 0;
    return ok;
}

static int key_before_nul(void) {
    static const char contents[] = "ssh-ed25519 testkey\0ignored\n";
    int ok;

    if (!save(contents, sizeof contents - 1, 0600)) return 0;
    ok = authorized();
    if (unlink("authorized_keys") == -1) return 0;
    return ok;
}

static int strict_nul(void) {
    static const char contents[] = "SSH-2.0-test\0ignored\n";
    char line[64];
    int p[2];
    int r;

    if (pipe(p) == -1) return 0;
    if (write(p[1], contents, sizeof contents - 1) !=
        (ssize_t) (sizeof contents - 1)) {
        close(p[0]);
        close(p[1]);
        return 0;
    }
    close(p[1]);
    errno = 0;
    r = getln(p[0], line, sizeof line);
    close(p[0]);
    return r == -1 && errno == EPROTO;
}

/* Disabled together with the / check: chroot root permissions depend on
   the build environment */
#if 0
static int root_checked(void) {
    static const char contents[] = "ssh-ed25519 otherkey\n";
    char logs[16384];
    ssize_t len;
    int p[2];
    int savederr;
    int ok;

    if (!save(contents, sizeof contents - 1, 0600)) return 0;
    if (pipe(p) == -1) return 0;
    savederr = dup(2);
    if (savederr == -1 || dup2(p[1], 2) == -1) return 0;
    close(p[1]);
    log_init(3, "test", 0, 0);
    ok = !authorized();
    if (dup2(savederr, 2) == -1) return 0;
    close(savederr);
    len = read(p[0], logs, sizeof logs - 1);
    close(p[0]);
    if (unlink("authorized_keys") == -1) return 0;
    if (len < 0) return 0;
    logs[len] = 0;
    return ok && strstr(logs, "auth: path: ok: / ") != 0;
}
#endif

static int result(const char *name, int ok) {
    printf("%s: %s\n", name, ok ? "ok" : "failed");
    return ok;
}

int main(void) {
    char dir[128];
    int ok = 1;

    snprintf(dir, sizeof dir, "subprocess-auth-test.%lu",
             (unsigned long) getpid());
    umask(0);
    if (mkdir(dir, 0700) == -1 || chdir(dir) == -1) return 111;
    log_init(-1, "test", 0, 0);

    ok &= result("descriptor identity", descriptor_identity());
    ok &= result("unsafe file", unsafe_file());
    ok &= result("fifo", fifo());
    ok &= result("safe symlink", safe_symlink());
    ok &= result("unsafe symlink", unsafe_symlink());
    ok &= result("NUL before key", nul_before_key());
    ok &= result("key before NUL", key_before_nul());
    ok &= result("strict NUL", strict_nul());
    /* Disabled together with the / check; see root_checked() above. */
    /* ok &= result("root directory", root_checked()); */

    if (chdir("..") == -1 || rmdir(dir) == -1) return 111;
    return ok ? 0 : 1;
}
