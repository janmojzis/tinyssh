/*
20140414
Jan Mojzis
Public domain.
*/

#include <fcntl.h>
#include "coe.h"

/*
The 'coe_enable(fd)' function enables
the close-on-exec flag on a file-descriptor 'fd'.
*/
void coe_enable(int fd) {
    fcntl(fd, F_SETFD, 1);
}

/*
The 'coe_disable(fd)' function disables
the close-on-exec flag on a file-descriptor 'fd'.
*/
void coe_disable(int fd) {
    fcntl(fd, F_SETFD, 0);
}
