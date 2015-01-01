/*
20140417
Jan Mojzis
Public domain.
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include "fail.h"
#include "open.h"
#include "randombytes.h"
#include "byte.h"
#include "load.h"
#include "writeall.h"

#define SPACELEN 1048577

unsigned char space1[SPACELEN];
unsigned char space2[SPACELEN];

static void test1(void) {

    int fd;


    fd = open_write("writealltest.data");
    if (fd == -1) fail("failure");
    randombytes(space1, sizeof space1);
    if (writeall(fd, space1, sizeof space1) == -1) fail("failure");
    close(fd);

    if (load("writealltest.data", space2, sizeof space2) == -1) fail("failure");

    if (!byte_isequal(space1, SPACELEN, space2)) fail("failure");

}

int main(void) {
    test1();
    _exit(0);
}
