/*
20140207
Jan Mojzis
Public domain.
*/

#include "uint32_pack_big.h"
#include "buf.h"
#include "packet.h"

static void packet_put_plain_(struct buf *b) {

    long long pos;
    crypto_uint8 paddinglen;
    struct buf *sendbuf = &packet.sendbuf;

    pos = sendbuf->len;                 /* get position */
    buf_putnum32(sendbuf, 0);           /* length */
    buf_putnum8(sendbuf, 0);            /* padding length */
    buf_put(sendbuf, b->buf, b->len);   /* add data */
    packet.sendpacketid++;              /* increment id */

    /* padding */
    paddinglen = 2 * 8 - ((sendbuf->len - pos) % 8);
    buf_putzerobytes(sendbuf, paddinglen);
    sendbuf->buf[pos + 4] = paddinglen;

    /* add packet length */
    uint32_pack_big(sendbuf->buf + pos, sendbuf->len - pos - 4);
}

int packet_putisready(void) {

    return buf_ready(&packet.sendbuf, PACKET_FULLLIMIT);
}


void packet_put(struct buf *b) {

    if (packet.flagkeys) {
        sshcrypto_packet_put(b);
    }
    else {
        packet_put_plain_(b);
    }
}
