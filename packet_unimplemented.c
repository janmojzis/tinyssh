/*
20150719
Jan Mojzis
Public domain.
*/

#include "buf.h"
#include "ssh.h"
#include "log.h"
#include "numtostr.h"
#include "packet.h"

int packet_unimplemented(struct buf *b) {

    char strnum[NUMTOSTR_LEN];

    /* note that b->buf[0] contains packetid */
    log_d3("packet=", numtostr(strnum, b->buf[0]),", sending SSH_MSG_UNIMPLEMENTED message");

    buf_purge(b);
    buf_putnum8(b, SSH_MSG_UNIMPLEMENTED);       /* SSH_MSG_UNIMPLEMENTED */
    buf_putnum32(b, packet.receivepacketid);     /* packeid */
    packet_put(b);
    return packet_sendall();
}
