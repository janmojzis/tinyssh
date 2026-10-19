#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdlib.h>
#include <unistd.h>

int main(void) {
    (void) closefrom(3);
    return 0;
}
