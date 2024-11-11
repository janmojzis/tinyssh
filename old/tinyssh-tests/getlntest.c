/*
20140323
Jan Mojzis
Public domain.
*/

#include <unistd.h>
#include <stdio.h>
#include "fail.h"
#include "savesync.h"
#include "str.h"
#include "byte.h"
#include "open.h"
#include "getln.h"

int main(void) {

    char buf[6];
    int fd;
    int r;

    /* create test file */
    if (savesync("getlntest.txt", "ab\ncd", 5) == -1) fail("savesync() failure");

    /* ok test */
    fd = open_read("getlntest.txt");
    if (fd == -1) fail("open_read() failure");
    r = getln(fd, buf, sizeof buf);
    if (r != 1) fail("getln() failure");
    if (str_len(buf) != 3) fail("getln() failure");
    if (!byte_isequal(buf, 3, "ab\n")) fail("getln() failure");
    r = getln(fd, buf, sizeof buf);
    if (r != 0) fail("getln() failure");
    if (str_len(buf) != 2) fail("getln() failure");
    if (!byte_isequal(buf, 2, "cd")) fail("getln() failure");
    close(fd);

    /* overflow test */
    fd = open_read("getlntest.txt");
    if (fd == -1) fail("open_read() failure");
    r = getln(fd, buf, 3);
    if (r != -1) fail("getln() failure");
    close(fd);
    if (unlink("getlntest.txt") == -1)  fail("unlink() failure");

    /* empty test */
    if (savesync("getlntest.txt", "", 0) == -1) fail("savesync() failure");
    fd = open_read("getlntest.txt");
    if (fd == -1) fail("open_read() failure");
    r = getln(fd, buf, sizeof buf);
    if (r != 0) fail("getln() failure");
    if (str_len(buf) != 0) fail("getln() failure");
    close(fd);
    if (unlink("getlntest.txt") == -1)  fail("unlink() failure");

    /* empty line test */
    if (savesync("getlntest.txt", "\n", 1) == -1) fail("savesync() failure");
    fd = open_read("getlntest.txt");
    if (fd == -1) fail("open_read() failure");
    r = getln(fd, buf, sizeof buf);
    if (r != 1) fail("getln() failure");
    if (str_len(buf) != 1) fail("getln() failure");
    r = getln(fd, buf, sizeof buf);
    if (r != 0) fail("getln() failure");
    if (str_len(buf) != 0) fail("getln() failure");
    close(fd);
    if (unlink("getlntest.txt") == -1)  fail("unlink() failure");

    _exit(0);
}

