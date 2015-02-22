/*
20140207
Jan Mojzis
Public domain.
*/

#include "buf.h"
#include "uint32_pack_big.h"
#include "uint32_unpack_big.h"
#include "randommod.h"
#include "packet.h"
#include "byte.h"
#include "bug.h"
#include "purge.h"
#include "e.h"
#include "crypto_verify_32.h"
#include "log.h"
#include "sshcrypto.h"

#define BB sshcrypto_cipher_blockbytes
#define AB sshcrypto_auth_bytes

void aesctr_packet_put(struct buf *b) {

    long long pos;
    crypto_uint8 paddinglen;
    struct buf *sendbuf = &packet.sendbuf;

    pos = sendbuf->len;                         /* get position       */
    buf_putnum32(sendbuf, 0);                   /* packet id, for MAC */
    buf_putnum32(sendbuf, 0);                   /* packet length      */
    buf_putnum8(sendbuf, 0);                    /* padding length     */
    buf_put(sendbuf, b->buf, b->len);           /* add data           */

    /* add padding */
    paddinglen  = 2 * BB - ((sendbuf->len - pos - 4) % BB);
    paddinglen += randommod(2) * BB;
    buf_putpadding(sendbuf, paddinglen);

    /* add space for mac */
    buf_putzerobytes(sendbuf, AB);

    /* add packet ID */
    uint32_pack_big(sendbuf->buf + pos, packet.sendpacketid++);

    /* add packet length */
    uint32_pack_big(sendbuf->buf + pos + 4, sendbuf->len - pos - AB - 4 - 4);

    /* add padding length*/
    sendbuf->buf[pos + 8] = paddinglen;

    /* get mac */
    if (sshcrypto_auth(
        sendbuf->buf + sendbuf->len - AB,       /* a    */
        sendbuf->buf + pos,                     /* m    */
        sendbuf->len - pos - AB,                /* mlen */
        packet.servermackey                     /* k    */
        ) != 0) bug_proto();

    /* encrypt */
    if (sshcrypto_stream_xor(
        sendbuf->buf + pos + 4,                 /* c    */
        sendbuf->buf + pos + 4,                 /* m    */
        sendbuf->len - pos - 4 - AB,            /* mlen */
        packet.servernonce,                     /* n    */
        packet.serverkey                        /* k    */
        ) != 0) bug_proto();


    /* remove packet id */
    byte_copy(sendbuf->buf + pos, sendbuf->len - pos - 4, sendbuf->buf + pos + 4);
    sendbuf->len -= 4;

    /* cleanup */
    purge(sendbuf->buf + sendbuf->len, 4);
}

#define ZB PACKET_ZEROBYTES

int aesctr_packet_get(struct buf *b) {

    long long len;
    struct buf *recvbuf = &packet.recvbuf;
    unsigned char *pp;
    long long l;

    /* we need at least one block */
    if (recvbuf->len - ZB < BB) { packet.packet_length = 0; return 1; }

    /* decrypt first block  */
    if (packet.packet_length == 0) {
        if (sshcrypto_stream_xor(
            recvbuf->buf + ZB,                  /* c    */
            recvbuf->buf + ZB,                  /* m    */
            BB,                                 /* mlen */
            packet.clientnonce,                 /* n    */
            packet.clientkey                    /* k    */
        ) != 0) bug_proto();
        packet.packet_length = uint32_unpack_big(recvbuf->buf + ZB);
    }

    if (packet.packet_length > PACKET_LIMIT) bug_proto();
    if (packet.packet_length + 4 + AB > recvbuf->len - ZB) return 1;


    /* decrypt and check MAC */
    uint32_pack_big(recvbuf->buf + ZB - 4, packet.receivepacketid++);
    if (sshcrypto_stream_xor(
            recvbuf->buf + ZB + BB,             /* c    */
            recvbuf->buf + ZB + BB,             /* m    */
            packet.packet_length + 4 - BB,      /* mlen */
            packet.clientnonce,                 /* n    */
            packet.clientkey                    /* k    */
        ) != 0) bug_proto();

    if (sshcrypto_auth(
            recvbuf->buf,                       /* a    */
            recvbuf->buf + ZB - 4,              /* m    */
            packet.packet_length + 8,           /* mlen */
            packet.clientmackey                 /* k    */
        ) != 0) bug_proto();

    if (crypto_verify_32(recvbuf->buf, recvbuf->buf + ZB + packet.packet_length + 4) != 0) bug_proto();

    len = packet.packet_length;
    len -= recvbuf->buf[ZB + 4] + 1;
    if (len <= 0) bug_proto();
    buf_put(b, recvbuf->buf + ZB + 5, len);

    pp = recvbuf->buf + ZB;
    l  = recvbuf->len - ZB;

    byte_copy(pp, l - packet.packet_length + AB + 4, pp + packet.packet_length + AB + 4);
    purge(pp + l - packet.packet_length + AB + 4, packet.packet_length + AB + 4);
    recvbuf->len -= packet.packet_length + AB + 4;
    packet.packet_length = 0;
    purge(recvbuf->buf, ZB);
    return 1;
}
