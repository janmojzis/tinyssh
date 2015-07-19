/*
20150719
Jan Mojzis
Public domain.
*/

#include "buf.h"
#include "ssh.h"
#include "packet.h"

int packet_uinmplemented(struct buf *b) {

    buf_purge(b);
    buf_putnum8(b, SSH_MSG_UNIMPLEMENTED);       /* SSH_MSG_UNIMPLEMENTED */
    buf_putnum32(b, packet.receivepacketid);     /* packeid */
    packet_put(b);
    return packet_sendall();
}
