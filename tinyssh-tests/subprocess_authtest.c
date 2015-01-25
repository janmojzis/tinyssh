/*
20140321
Jan Mojzis
Public domain.
*/

#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <pwd.h>
#include "fail.h"
#include "dropuidgid.h"
#include "savesync.h"
#include "open.h"
#include "global.h"
#include "byte.h"
#include "str.h"
#include "log.h"
#include "subprocess.h"

static int readall(int fd, unsigned char *x, long long xlen) {

    long long r;
    long long len = 0;

    while (xlen > 0) {
        r = read(fd, x, xlen);
        if (r == 0) break;
        if (r <= 0) return -1;
        x += r;
        len += r;
        xlen -= r;
    }
    return len;
}

static unsigned char logbuf[1024];
static char globalpath[4096];
static long long globalpathlen;

static void run(void (*op)(void), const char *exp) {

    pid_t pid;
    int status, fromchild[2];
    long long r, i;

    if (pipe(fromchild) == -1) fail("pipe() failure");
    pid = fork();
    if (pid == -1) fail("fork() failure");
    if (pid == 0) {
        close(fromchild[0]);
        if (dup2(fromchild[1], 2) == -1) _exit(111);
        op();
        _exit(0);
    }
    close(fromchild[1]);
    r = readall(fromchild[0], logbuf, sizeof logbuf);
    if (r == -1) fail("read() failure");

    for (i = 0; i < r; ++i) if (logbuf[i] == '\n') break;
    r = i;
    for (i = 0; i < r; ++i) if (logbuf[i] == '/') break;
    r -= i;
    byte_copy(logbuf, r, logbuf + i);

    /* fprintf(stderr, "xxx: %s\n", logbuf); fflush(stderr); */

    if (r < globalpathlen + 1) fail("log error");
    if (!byte_isequal(globalpath, globalpathlen, logbuf)) fail("log error");
    r -= globalpathlen + 1;
    byte_copy(logbuf, r, logbuf + globalpathlen + 1);

    for (i = 0; i < r; ++i) if (logbuf[i] == '{') break;
    r = i;
    logbuf[r] = 0;

    while (waitpid(pid, &status, 0) != pid) {};
    if (!WIFEXITED(status)) fail("process killed");
    if (WEXITSTATUS(status)) fail("process exited with status != 0");

    i = str_len(exp);
    if (r != i) fail("failed");
    if (!byte_isequal(logbuf, i, exp)) fail("failed");
}

static int run2(void (*op)(void)) {

    pid_t pid;
    int status;

    pid = fork();
    if (pid == -1) return -1;
    if (pid == 0) {
        close(2);
        op();
        _exit(0);
    }

    while (waitpid(pid, &status, 0) != pid) {};
    if (!WIFEXITED(status)) return -1;
    return WEXITSTATUS(status);
}



static void test_usernotexist(void) {

    char *account = "21ecdfcc00506bc138d004a0c04139442e24b02ac456bf05601f1e8f645baa2";
    char *keyname = "ssh-ed25519";
    char *key = "key1";

    if (subprocess_auth(account, keyname, key) == 0) fail("subprocess_auth() failure");
}

static void test_usertoolong(void) {

    char *account = "21ecdfcc00506bc138d004a0c04139442e24b02ac456bf05601f1e8f645baa25";
    char *keyname = "ssh-ed25519";
    char *key = "key1";

    if (subprocess_auth(account, keyname, key) == 0) fail("subprocess_auth() failure");
}

static void test_keytooshort(void) {

    char *account = "21ecdfcc00506bc138d004a0c04139442e24b02ac456bf05601f1e8f645baa2";
    char *keyname = "ssh-ed25519";
    char *key = "AAAAC3NzaC1lZDI1NTE5AAAAICj44ZR+OCpjuLbOwqys2MKHroSvAWGgEE1o7yq+UVe";

    if (subprocess_auth(account, keyname, key) == 0) fail("subprocess_auth() failure");
}


static struct passwd *pw;

static void droproot(void) {

    uid_t uid;

    uid = geteuid();
    if (uid == 0) {
        pw = getpwnam("nobody");
        if (!pw) fail("getpwnam() failure")
        if (!dropuidgid(pw->pw_uid, pw->pw_gid)) fail("dropuidgid() failure");
    }
    else {
        pw = getpwuid(uid);
        if (!pw) fail("getpwuid() failure")
    }
    if (geteuid() == 0) fail("dropuidgid() failure");
}

static void test_root(void) {

    char *account = "root";
    char *keyname = "ssh-ed25519";
    char *key = "key1";

    if (subprocess_auth(account, keyname, key) == 0) fail("subprocess_auth() failure");
}

extern int subprocess_auth_checkpath_(char *, long long, uid_t);

#define path global_bspace2 /* reusing global buffer */
#define buf global_bspace1 /* reusing global buffer */

static void test_path_authorizedkeys_ne(void) {

    umask(000);
    if (mkdir("d1", 0755) == -1) fail("mkdir() failure");
    if (chdir("d1") == -1) fail("chdir() failure");
    if (subprocess_auth_checkpath_((char *)path, sizeof path, geteuid())) fail("subprocess_auth_checkpath_() failure");
    if (chdir("..") == -1) fail("chdir() failure");
    if (rmdir("d1") == -1) fail("rmdir() failure");
}

static void test_path_authorizedkeys_fifo(void) {

    umask(000);
    if (mkdir("d1", 0755) == -1) fail("mkdir() failure");
    if (chdir("d1") == -1) fail("chdir() failure");
    if (mkfifo("authorized_keys", 0644) == -1) fail("mkfifo() failure");
    if (subprocess_auth_checkpath_((char *)path, sizeof path, geteuid())) fail("subprocess_auth_checkpath_() failure");
    if (unlink("authorized_keys") == -1) fail("unlink() failure");
    if (chdir("..") == -1) fail("chdir() failure");
    if (rmdir("d1") == -1) fail("rmdir() failure");
}

static void test_path_authorizedkeys_perm1(void) {

    umask(000);
    if (mkdir("d1", 0755) == -1) fail("mkdir() failure");
    if (chdir("d1") == -1) fail("chdir() failure");
    if (savesync("authorized_keys", "", 0) == -1) fail("savesync() failure");
    if (chmod("authorized_keys", 0777) == -1) fail("chmod() failure");
    if (subprocess_auth_checkpath_((char *)path, sizeof path, geteuid())) fail("subprocess_auth_checkpath_() failure");
    if (unlink("authorized_keys") == -1) fail("unlink() failure");
    if (chdir("..") == -1) fail("chdir() failure");
    if (rmdir("d1") == -1) fail("rmdir() failure");
}
static void test_path_authorizedkeys_perm2(void) {

    umask(000);
    if (mkdir("d1", 0755) == -1) fail("mkdir() failure");
    if (chdir("d1") == -1) fail("chdir() failure");
    if (savesync("authorized_keys", "", 0) == -1) fail("savesync() failure");
    if (chmod("authorized_keys", 0775) == -1) fail("chmod() failure");
    if (subprocess_auth_checkpath_((char *)path, sizeof path, geteuid())) fail("subprocess_auth_checkpath_() failure");
    if (unlink("authorized_keys") == -1) fail("unlink() failure");
    if (chdir("..") == -1) fail("chdir() failure");
    if (rmdir("d1") == -1) fail("rmdir() failure");
}
static void test_path_authorizedkeys_perm3(void) {

    umask(000);
    if (mkdir("d1", 0755) == -1) fail("mkdir() failure");
    if (chdir("d1") == -1) fail("chdir() failure");
    if (savesync("authorized_keys", "", 0) == -1) fail("savesync() failure");
    if (chmod("authorized_keys", 0757) == -1) fail("chmod() failure");
    if (subprocess_auth_checkpath_((char *)path, sizeof path, geteuid())) fail("subprocess_auth_checkpath_() failure");
    if (unlink("authorized_keys") == -1) fail("unlink() failure");
    if (chdir("..") == -1) fail("chdir() failure");
    if (rmdir("d1") == -1) fail("rmdir() failure");
}


static void test_path_dir_perm1(void) {

    umask(000);
    if (mkdir("d1", 0775) == -1) fail("mkdir() failure");
    if (chdir("d1") == -1) fail("chdir() failure");
    if (savesync("authorized_keys", "", 0) == -1) fail("savesync() failure");
    if (subprocess_auth_checkpath_((char *)path, sizeof path, geteuid())) fail("subprocess_auth_checkpath_() failure");
    if (unlink("authorized_keys") == -1) fail("unlink() failure");
    if (chdir("..") == -1) fail("chdir() failure");
    if (rmdir("d1") == -1) fail("rmdir() failure");
}
static void test_path_dir_perm2(void) {

    umask(000);
    if (mkdir("d1", 0777) == -1) fail("mkdir() failure");
    if (chdir("d1") == -1) fail("chdir() failure");
    if (savesync("authorized_keys", "", 0) == -1) fail("savesync() failure");
    if (subprocess_auth_checkpath_((char *)path, sizeof path, geteuid())) fail("subprocess_auth_checkpath_() failure");
    if (unlink("authorized_keys") == -1) fail("unlink() failure");
    if (chdir("..") == -1) fail("chdir() failure");
    if (rmdir("d1") == -1) fail("rmdir() failure");
}
static void test_path_dir_perm3(void) {

    umask(000);
    if (mkdir("d1", 0757) == -1) fail("mkdir() failure");
    if (chdir("d1") == -1) fail("chdir() failure");
    if (savesync("authorized_keys", "", 0) == -1) fail("savesync() failure");
    if (subprocess_auth_checkpath_((char *)path, sizeof path, geteuid())) fail("subprocess_auth_checkpath_() failure");
    if (unlink("authorized_keys") == -1) fail("unlink() failure");
    if (chdir("..") == -1) fail("chdir() failure");
    if (rmdir("d1") == -1) fail("rmdir() failure");
}

static void test_path_dir2_perm1(void) {

    umask(000);
    if (mkdir("d1", 0777) == -1) fail("mkdir() failure");
    if (chdir("d1") == -1) fail("chdir() failure");
    if (mkdir("d2", 0777) == -1) fail("mkdir() failure");
    if (chdir("d2") == -1) fail("chdir() failure");
    if (savesync("authorized_keys", "", 0) == -1) fail("savesync() failure");
    if (subprocess_auth_checkpath_((char *)path, sizeof path, geteuid())) fail("subprocess_auth_checkpath_() failure");
    if (unlink("authorized_keys") == -1) fail("unlink() failure");
    if (chdir("..") == -1) fail("chdir() failure");
    if (rmdir("d2") == -1) fail("rmdir() failure");
    if (chdir("..") == -1) fail("chdir() failure");
    if (rmdir("d1") == -1) fail("rmdir() failure");
}
static void test_path_dir2_perm2(void) {

    umask(000);
    if (mkdir("d1", 0777) == -1) fail("mkdir() failure");
    if (chdir("d1") == -1) fail("chdir() failure");
    if (mkdir("d2", 0775) == -1) fail("mkdir() failure");
    if (chdir("d2") == -1) fail("chdir() failure");
    if (savesync("authorized_keys", "", 0) == -1) fail("savesync() failure");
    if (subprocess_auth_checkpath_((char *)path, sizeof path, geteuid())) fail("subprocess_auth_checkpath_() failure");
    if (unlink("authorized_keys") == -1) fail("unlink() failure");
    if (chdir("..") == -1) fail("chdir() failure");
    if (rmdir("d2") == -1) fail("rmdir() failure");
    if (chdir("..") == -1) fail("chdir() failure");
    if (rmdir("d1") == -1) fail("rmdir() failure");
}
static void test_path_dir2_perm3(void) {

    umask(000);
    if (mkdir("d1", 0777) == -1) fail("mkdir() failure");
    if (chdir("d1") == -1) fail("chdir() failure");
    if (mkdir("d2", 0757) == -1) fail("mkdir() failure");
    if (chdir("d2") == -1) fail("chdir() failure");
    if (savesync("authorized_keys", "", 0) == -1) fail("savesync() failure");
    if (subprocess_auth_checkpath_((char *)path, sizeof path, geteuid())) fail("subprocess_auth_checkpath_() failure");
    if (unlink("authorized_keys") == -1) fail("unlink() failure");
    if (chdir("..") == -1) fail("chdir() failure");
    if (rmdir("d2") == -1) fail("rmdir() failure");
    if (chdir("..") == -1) fail("chdir() failure");
    if (rmdir("d1") == -1) fail("rmdir() failure");
}

static void test_path_dir3_perm1(void) {

    umask(000);
    if (mkdir("d1", 0777) == -1) fail("mkdir() failure");
    if (chdir("d1") == -1) fail("chdir() failure");
    if (mkdir("d2", 0755) == -1) fail("mkdir() failure");
    if (chdir("d2") == -1) fail("chdir() failure");
    if (savesync("authorized_keys", "", 0) == -1) fail("savesync() failure");
    if (subprocess_auth_checkpath_((char *)path, sizeof path, geteuid())) fail("subprocess_auth_checkpath_() failure");
    if (unlink("authorized_keys") == -1) fail("unlink() failure");
    if (chdir("..") == -1) fail("chdir() failure");
    if (rmdir("d2") == -1) fail("rmdir() failure");
    if (chdir("..") == -1) fail("chdir() failure");
    if (rmdir("d1") == -1) fail("rmdir() failure");
}
static void test_path_dir3_perm2(void) {

    umask(000);
    if (mkdir("d1", 0775) == -1) fail("mkdir() failure");
    if (chdir("d1") == -1) fail("chdir() failure");
    if (mkdir("d2", 0755) == -1) fail("mkdir() failure");
    if (chdir("d2") == -1) fail("chdir() failure");
    if (savesync("authorized_keys", "", 0) == -1) fail("savesync() failure");
    if (subprocess_auth_checkpath_((char *)path, sizeof path, geteuid())) fail("subprocess_auth_checkpath_() failure");
    if (unlink("authorized_keys") == -1) fail("unlink() failure");
    if (chdir("..") == -1) fail("chdir() failure");
    if (rmdir("d2") == -1) fail("rmdir() failure");
    if (chdir("..") == -1) fail("chdir() failure");
    if (rmdir("d1") == -1) fail("rmdir() failure");
}
static void test_path_dir3_perm3(void) {

    umask(000);
    if (mkdir("d1", 0757) == -1) fail("mkdir() failure");
    if (chdir("d1") == -1) fail("chdir() failure");
    if (mkdir("d2", 0755) == -1) fail("mkdir() failure");
    if (chdir("d2") == -1) fail("chdir() failure");
    if (savesync("authorized_keys", "", 0) == -1) fail("savesync() failure");
    if (subprocess_auth_checkpath_((char *)path, sizeof path, geteuid())) fail("subprocess_auth_checkpath_() failure");
    if (unlink("authorized_keys") == -1) fail("unlink() failure");
    if (chdir("..") == -1) fail("chdir() failure");
    if (rmdir("d2") == -1) fail("rmdir() failure");
    if (chdir("..") == -1) fail("chdir() failure");
    if (rmdir("d1") == -1) fail("rmdir() failure");
}

static void test_path_dir_symlink(void) {

    umask(000);
    if (mkdir("d1", 0775) == -1) fail("mkdir() failure");
    if (symlink("d1", "d2") == -1) fail("symlink() failure");
    if (chdir("d2") == -1) fail("chdir() failure");
    if (savesync("authorized_keys", "", 0) == -1) fail("savesync() failure");
    if (subprocess_auth_checkpath_((char *)path, sizeof path, geteuid())) fail("subprocess_auth_checkpath_() failure");
    if (unlink("authorized_keys") == -1) fail("unlink() failure");
    if (chdir("..") == -1) fail("chdir() failure");
    if (unlink("d2") == -1) fail("rmdir() failure");
    if (rmdir("d1") == -1) fail("rmdir() failure");
}

extern int subprocess_auth_authorizedkeys_(char *, char *, char *, char *, long long);

static void test_authorizedkeys_ne(void) {

    char *keyname = "";
    char *key = "";
    if (!getcwd((char *)path, sizeof path)) fail("getcwd() failure");

    if (subprocess_auth_authorizedkeys_(keyname, key, (char *)path, (char *)buf, sizeof buf))
        fail("subprocess_auth_authorizedkeys_() failure");
}

#define AUTHORIZED_KEYS "\
\n\
\n\
###\n\
\n\
\n\
\n\
\n\
ssh-rsa AAAAB3Nza...LiPk== user@example.net\n\
from=\"*.sales.example.net,!pc.sales.example.net\" ssh-rsa AAAAB2...19Q== john@example.net\n\
command=\"dump /home\",no-pty,no-port-forwarding ssh-dss AAAAC3...51R== example.net\n\
permitopen=\"192.0.2.1:80\",permitopen=\"192.0.2.2:25\" ssh-dss AAAAB5...21S==\n\
tunnel=\"0\",command=\"sh /etc/netstart tun0\" ssh-rsa AAAA...== jane@example.net\n\
ecdsa-sha2-nistp256 ecdsakey1 apache@apache\n\
\n\
\n\
 ssh-ed25519 badkey1 note - extra ' ' before keyname\n\
ssh-ed25519 badkey2. note - '.' as separator\n\
ssh-ed255191 badkey3\n\
ssh-ed25519\tbadkey4\n\
ssh-ed25519  badkey5\n\
\n\
\n\
ssh-ed25519 key1\n\
ssh-ed25519 key2\r\n\
ssh-ed25519 key3 note - ' ' as separator\n\
ssh-ed25519 key4\0note - '\0' as separator\n\
ssh-ed25519 key5 note - last line"


static void authorizedkeys(void) {
    if (savesync("authorized_keys", AUTHORIZED_KEYS, sizeof(AUTHORIZED_KEYS) - 1) == -1) fail("savesync() failure");

}

static void test_authorizedkeys_ok(void) {

    char *keyname, *key;

    if (!getcwd((char *)path, sizeof path)) fail("getcwd() failure");

    keyname = "ssh-ed25519"; key = "key1";
    if (!subprocess_auth_authorizedkeys_(keyname, key, (char *)path, (char *)buf, sizeof buf))
        fail("subprocess_auth_authorizedkeys_() failure");

    keyname = "ssh-ed25519"; key = "key2";
    if (!subprocess_auth_authorizedkeys_(keyname, key, (char *)path, (char *)buf, sizeof buf))
        fail("subprocess_auth_authorizedkeys_() failure");

    keyname = "ssh-ed25519"; key = "key3";
    if (!subprocess_auth_authorizedkeys_(keyname, key, (char *)path, (char *)buf, sizeof buf))
        fail("subprocess_auth_authorizedkeys_() failure");

    keyname = "ssh-ed25519"; key = "key4";
    if (!subprocess_auth_authorizedkeys_(keyname, key, (char *)path, (char *)buf, sizeof buf))
        fail("subprocess_auth_authorizedkeys_() failure");

    keyname = "ssh-ed25519"; key = "key5";
    if (!subprocess_auth_authorizedkeys_(keyname, key, (char *)path, (char *)buf, sizeof buf))
        fail("subprocess_auth_authorizedkeys_() failure");
}

static void test_authorizedkeys_bad(void) {

    char *keyname, *key;

    if (!getcwd((char *)path, sizeof path)) fail("getcwd() failure");

    keyname = "ssh-ed25519"; key = "badkey1";
    if (subprocess_auth_authorizedkeys_(keyname, key, (char *)path, (char *)buf, sizeof buf))
        fail("subprocess_auth_authorizedkeys_() failure");

    keyname = "ssh-ed25519"; key = "badkey2";
    if (subprocess_auth_authorizedkeys_(keyname, key, (char *)path, (char *)buf, sizeof buf))
        fail("subprocess_auth_authorizedkeys_() failure");

    keyname = "ssh-ed25519"; key = "badkey3";
    if (subprocess_auth_authorizedkeys_(keyname, key, (char *)path, (char *)buf, sizeof buf))
        fail("subprocess_auth_authorizedkeys_() failure");

    keyname = "ssh-ed25519"; key = "badkey4";
    if (subprocess_auth_authorizedkeys_(keyname, key, (char *)path, (char *)buf, sizeof buf))
        fail("subprocess_auth_authorizedkeys_() failure");

    keyname = "ssh-ed25519"; key = "badkey5";
    if (subprocess_auth_authorizedkeys_(keyname, key, (char *)path, (char *)buf, sizeof buf))
        fail("subprocess_auth_authorizedkeys_() failure");
}



int main(void) {

    if (!getcwd(globalpath, sizeof globalpath)) fail("getcwd() failure");
    globalpathlen = str_len(globalpath);

    run(test_path_authorizedkeys_ne, "d1/authorized_keys (file does not exist)");
    run(test_path_authorizedkeys_fifo, "d1/authorized_keys (access denied)");
    run(test_path_authorizedkeys_perm1, "d1/authorized_keys (access denied)");
    run(test_path_authorizedkeys_perm2, "d1/authorized_keys (access denied)");
    run(test_path_authorizedkeys_perm3, "d1/authorized_keys (access denied)");
    run(test_path_dir_perm1, "d1/ (access denied)");
    run(test_path_dir_perm2, "d1/ (access denied)");
    run(test_path_dir_perm3, "d1/ (access denied)");
    run(test_path_dir2_perm1, "d1/d2/ (access denied)");
    run(test_path_dir2_perm2, "d1/d2/ (access denied)");
    run(test_path_dir2_perm3, "d1/d2/ (access denied)");
    run(test_path_dir3_perm1, "d1/ (access denied)");
    run(test_path_dir3_perm2, "d1/ (access denied)");
    run(test_path_dir3_perm3, "d1/ (access denied)");
    run(test_path_dir_symlink, "d1/ (access denied)");

    log_init(-1, "xxx", 1, 0);
    test_authorizedkeys_ne();
    authorizedkeys();
    test_authorizedkeys_ok();
    test_authorizedkeys_bad();

    droproot();
    run2(test_usernotexist);
    run2(test_usertoolong);
    run2(test_keytooshort);
    run2(test_root);
    _exit(0);
}
