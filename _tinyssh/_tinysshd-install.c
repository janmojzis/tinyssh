#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include "open.h"
#include "log.h"


static void usage(void) {

    log_u1("_tinysshd-install: usage: _tinysshd-install sourcefile destdir tempfile destfile [flagexecutable]");
    _exit(100);
}

static int flagtmpcreated = 0;
static const char *srcfn = 0;
static const char *tmpfn = 0;
static const char *dstfn = 0;
static const char *dstdir = 0;
static int fd1 = -1, fd2 = -1;

static void cleanup(void) {
    if (flagtmpcreated) {
        unlink(tmpfn);
    }
}

static void die_fatal(const char *trouble, const char *d, const char *fn) {

    cleanup();

    if (d) {
        if (fn) log_f5(trouble, " ", d, "/", fn);
        else log_f3(trouble, " ", d);
    }
    else {
        log_f1(trouble);
    }
    _exit(111);
}

static int flagexecutable = 0;

int main(int argc, char **argv) {

    struct stat st;
    long long r;
    char ch;

    log_init(2, "_tinysshd-install", 0, 0);

    if (argc < 5) usage();
    if (!argv[0]) usage();
    if (!argv[1]) usage();
    if (!argv[2]) usage();
    if (!argv[3]) usage();
    if (!argv[4]) usage();

    srcfn = argv[1];
    dstdir  = argv[2];
    tmpfn = argv[3];
    dstfn = argv[4];
    if (argv[5]) flagexecutable = 1;

    /* open source file */
    fd1 = open_read(srcfn);
    if (fd1 == -1) die_fatal("unable to open file", srcfn, 0);

    /* stat destination directory and change directory */
    if ((stat(dstdir, &st)) == -1) die_fatal("unable to stat directory", dstdir, 0);
    if (chdir(dstdir) == -1) die_fatal("unable to change directory to", dstdir, 0);

    /* open temporary file */
    fd2 = open_write(tmpfn);
    if (fd2 == -1) die_fatal("unable to open file", dstdir, tmpfn);
    flagtmpcreated = 1;

    /* copy file */
    for (;;) {
        r = read(fd1, &ch, 1);
        if (r == -1) die_fatal("unable to read from file", srcfn, 0);
        if (r == 0) break;
        if (write(fd2, &ch, 1) != 1) die_fatal("unable to write to file", dstdir, tmpfn);
    }
    if (fsync(fd2) == -1) die_fatal("unable to write to file", dstdir, tmpfn);
    if (fchown(fd2, st.st_uid, st.st_gid) == -1) die_fatal("unable to change owner on", dstdir, tmpfn);
    if (flagexecutable) {
        if (fchmod(fd2, 0755) == -1) die_fatal("unable to change owner on", dstdir, tmpfn);
    }
    else {
        if (fchmod(fd2, 0644) == -1) die_fatal("unable to change owner on", dstdir, tmpfn);
    }
    if (close(fd2) == -1) die_fatal("unable to write to file", dstdir, tmpfn);
    if (rename(tmpfn, dstfn) == -1) die_fatal("unable to rename file to", dstdir, dstfn);
    _exit(0);
}
