/*
20140210
Jan Mojzis
Public domain.
*/

#include "bug.h"
#include "buf.h"
#include "channel.h"
#include "ssh.h"
#include "packetparser.h"
#include "packet.h"

int packet_channel_recv_data(struct buf *b) {

    long long pos = 0;
    crypto_uint32 len, id;
    crypto_uint8 ch;

    pos = packetparser_uint8(b->buf, b->len, pos, &ch);         /* byte      SSH_MSG_CHANNEL_DATA */
    if (ch != SSH_MSG_CHANNEL_DATA) bug_proto();
    pos = packetparser_uint32(b->buf, b->len, pos, &id);        /* uint32    recipient channel    */
    if (id != channel_getid()) bug_proto();
    pos = packetparser_uint32(b->buf, b->len, pos, &len);       /* string    data                 */
    pos = packetparser_skip(b->buf, b->len, pos, len);
    pos = packetparser_end(b->buf, b->len, pos);

    channel_put(b->buf + pos - len, len);
    buf_purge(b);
    return 1;
}

int packet_channel_recv_extendeddata(struct buf *b) {

    /* ignore extended data */
    buf_purge(b);
    return 1;
}


int packet_channel_recv_windowadjust(struct buf *b) {

    long long pos = 0;
    crypto_uint32 len, id;
    crypto_uint8 ch;

    pos = packetparser_uint8(b->buf, b->len, pos, &ch);       /* byte      SSH_MSG_CHANNEL_WINDOW_ADJUST */
    if (ch != SSH_MSG_CHANNEL_WINDOW_ADJUST) bug_proto();
    pos = packetparser_uint32(b->buf, b->len, pos, &id);      /* uint32    recipient channel             */
    if (id != channel_getid()) bug_proto();
    pos = packetparser_uint32(b->buf, b->len, pos, &len);     /* uint32    bytes to add                  */
    pos = packetparser_end(b->buf, b->len, pos);

    channel_incrementremotewindow(len);
    buf_purge(b);
    return 1;
}


int packet_channel_recv_eof(struct buf *b) {

    long long pos = 0;
    crypto_uint32 id;
    crypto_uint8 ch;

    pos = packetparser_uint8(b->buf, b->len, pos, &ch);       /* byte      SSH_MSG_CHANNEL_EOF */
    if (ch != SSH_MSG_CHANNEL_EOF) bug_proto();
    pos = packetparser_uint32(b->buf, b->len, pos, &id);      /* uint32    recipient channel   */
    if (id != channel_getid()) bug_proto();
    pos = packetparser_end(b->buf, b->len, pos);

    channel_puteof();
    buf_purge(b);
    return 1;
}


int packet_channel_recv_close(struct buf *b) {

    long long pos = 0;
    crypto_uint32 id;
    crypto_uint8 ch;

    pos = packetparser_uint8(b->buf, b->len, pos, &ch);       /* byte      SSH_MSG_CHANNEL_CLOSE */
    if (ch != SSH_MSG_CHANNEL_CLOSE) bug_proto();
    pos = packetparser_uint32(b->buf, b->len, pos, &id);      /* uint32    recipient channel   */
    if (id != channel_getid()) bug_proto();
    pos = packetparser_end(b->buf, b->len, pos);

    packet_channel_send_eof(b);
    packet.flagchanneleofreceived = 1;
    buf_purge(b);
    return 1;
}
