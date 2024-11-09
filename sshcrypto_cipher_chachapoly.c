/*
20140207
Jan Mojzis
Public domain.
*/

#include "uint32_pack_big.h"
#include "uint32_unpack_big.h"
#include "crypto_verify_16.h"
#include "randommod.h"
#include "e.h"
#include "byte.h"
#include "purge.h"
#include "sshcrypto.h"
#include "bug.h"
#include "packet.h"

#define BB sshcrypto_cipher_blockbytes
#define AB sshcrypto_auth_bytes
#define ZB 60

void chachapoly_packet_put(struct buf *b) {

    long long pos;
    crypto_uint8 paddinglen;
    struct buf *sendbuf = &packet.sendbuf;
    unsigned char n[8];

    pos = sendbuf->len;                         /* get position   */
    buf_putzerobytes(sendbuf, ZB);              /* zero bytes     */ 
    buf_putnum32(sendbuf, 0);                   /* the length     */
    buf_putnum8(sendbuf, 0);                    /* padding length */
    buf_put(sendbuf, b->buf, b->len);           /* add data       */

    /* pack nonce */
    byte_zero(n, 4);
    uint32_pack_big(n + 4, packet.sendpacketid++);

    /* padding */
    paddinglen  = 2 * BB - ((sendbuf->len - pos - ZB) % BB) + 4;
    paddinglen += randommod(2) * BB;
    buf_putpadding(sendbuf, paddinglen);
    sendbuf->buf[pos + ZB + 4] = paddinglen;

    /* space for mac  */
    buf_putzerobytes(sendbuf, AB);

    /* encrypt data */
    sshcrypto_stream_xor(sendbuf->buf + pos, sendbuf->buf + pos, sendbuf->len - pos - AB, n, packet.serverkey);

    /* add packet length */
    uint32_pack_big(sendbuf->buf + pos + ZB, sendbuf->len - pos - AB - 4 - ZB);

    /* encrypt the length */
    sshcrypto_stream_xor(sendbuf->buf + pos + ZB, sendbuf->buf + pos + ZB, 4, n, packet.serverkey + 32);
    purge(n, sizeof n);

    /* authenticate data */
    sshcrypto_auth(sendbuf->buf + sendbuf->len - AB, sendbuf->buf + pos + ZB, sendbuf->len - AB - pos - ZB, sendbuf->buf + pos);

    /* remove zerobytes */
    byte_copy(sendbuf->buf + pos, sendbuf->len - pos - ZB, sendbuf->buf + pos + ZB);
    sendbuf->len -= ZB;
    purge(sendbuf->buf + sendbuf->len, ZB);
}


int chachapoly_packet_get(struct buf *b) {

    long long len;
    struct buf *recvbuf = &packet.recvbuf;
    unsigned char buf[16];
    unsigned char *pp;
    long long l;
    unsigned char n[8];

    /* we need at least 4 bytes */
    if (recvbuf->len - PACKET_ZEROBYTES < 4) { packet.packet_length = 0; return 1; }

    /* parse length */
    byte_zero(n, 4);
    uint32_pack_big(n + 4, packet.receivepacketid);
    if (packet.packet_length == 0) {
        sshcrypto_stream_xor(buf, recvbuf->buf + PACKET_ZEROBYTES, 4, n, packet.clientkey + 32);
        packet.packet_length = uint32_unpack_big(buf);
    }

    if (packet.packet_length > PACKET_LIMIT) bug_proto();
    if (packet.packet_length + AB + 4 > recvbuf->len - PACKET_ZEROBYTES) return 1;


    /* verify and decrypt packet */
    byte_zero(recvbuf->buf, 32);
    sshcrypto_stream_xor(recvbuf->buf, recvbuf->buf, 32, n, packet.clientkey);
    sshcrypto_auth(buf, recvbuf->buf + PACKET_ZEROBYTES, packet.packet_length + 4, recvbuf->buf);
    if (crypto_verify_16(buf, recvbuf->buf + PACKET_ZEROBYTES + packet.packet_length + 4) != 0) bug_proto();
    purge(buf, sizeof buf);
    sshcrypto_stream_xor(recvbuf->buf + 4, recvbuf->buf + 4, packet.packet_length + PACKET_ZEROBYTES, n, packet.clientkey);
    purge(n, sizeof n);

    /* process packet */
    len = packet.packet_length;
    len -= recvbuf->buf[PACKET_ZEROBYTES + 4] + 1;
    if (len <= 0) bug_proto();
    buf_put(b, recvbuf->buf + PACKET_ZEROBYTES + 5, len);

    pp = recvbuf->buf + PACKET_ZEROBYTES;
    l  = recvbuf->len - PACKET_ZEROBYTES;

    byte_copy(pp, l - packet.packet_length + AB + 4, pp + packet.packet_length + AB + 4);
    purge(pp + l - packet.packet_length + AB + 4, packet.packet_length + AB + 4);
    recvbuf->len -= packet.packet_length + AB + 4;

    /* packetid */
    packet.receivepacketid++;

    /* cleanup */
    packet.packet_length = 0;
    purge(recvbuf->buf, PACKET_ZEROBYTES);
    return 1;
}
