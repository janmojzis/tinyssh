#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
extern int rename(const char *, const char *);
#include "open.h"
#include "log.h"


static void usage(void) {

    log_u1("_tinysshd-install: usage: _tinysshd-install sourcefile destdir tempfile destfile");
    _exit(100);
}

static int flagtmpcreated = 0;
static const char *srcfile = 0;
static const char *tmpfile = 0;
static const char *dstfile = 0;
static const char *dstdir = 0;
static int fd1 = -1, fd2 = -1;

static void cleanup(void) {
    if (flagtmpcreated) {
        unlink(tmpfile);
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

    srcfile = argv[1];
    dstdir  = argv[2];
    tmpfile = argv[3];
    dstfile = argv[4];

    /* open source file */
    fd1 = open_read(srcfile);
    if (fd1 == -1) die_fatal("unable to open file", srcfile, 0);

    /* stat destination directory and change directory */
    if ((stat(dstdir, &st)) == -1) die_fatal("unable to stat directory", dstdir, 0);
    if (chdir(dstdir) == -1) die_fatal("unable to change directory to", dstdir, 0);

    /* open temporary file */
    fd2 = open_write(tmpfile);
    if (fd2 == -1) die_fatal("unable to open file", dstdir, tmpfile);
    flagtmpcreated = 1;

    /* copy file */
    for (;;) {
        r = read(fd1, &ch, 1);
        if (r == -1) die_fatal("unable to read from file", srcfile, 0);
        if (r == 0) break;
        if (write(fd2, &ch, 1) != 1) die_fatal("unable to write to file", dstdir, tmpfile);
    }
    if (fsync(fd2) == -1) die_fatal("unable to write to file", dstdir, tmpfile);
    if (fchown(fd2, st.st_uid, st.st_gid) == -1) die_fatal("unable to change owner on", dstdir, tmpfile);
    if (fchmod(fd2, 0755) == -1) die_fatal("unable to change owner on", dstdir, tmpfile);
    if (close(fd2) == -1) die_fatal("unable to write to file", dstdir, tmpfile);
    if (rename(tmpfile, dstfile) == -1) die_fatal("unable to rename file to", dstdir, dstfile);
    _exit(0);
}
