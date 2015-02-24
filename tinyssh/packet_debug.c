/*
20150218
Jan Mojzis
Public domain.
*/

#include "buf.h"
#include "ssh.h"
#include "packet.h"

void packet_debug_send(struct buf *b, char *x, long long xlen) {

    if (xlen < 0) return;
    if (!packet.flagkeys) return;

    buf_purge(b);
    buf_putnum8(b, SSH_MSG_DEBUG);
    buf_putnum8(b, 1);
    buf_putstringlen(b, (unsigned char *)x, xlen);
    buf_putstring(b, "none");
    packet_put(b);
    buf_purge(b);
}
