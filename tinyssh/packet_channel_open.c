/*
20140120
Jan Mojzis
Public domain.
*/

#include "buf.h"
#include "ssh.h"
#include "e.h"
#include "bug.h"
#include "packetparser.h"
#include "str.h"
#include "packet.h"

int packet_channel_open(struct buf *b1, struct buf *b2) {

    crypto_uint32 id, remotewindow, localwindow, maxpacket, chanlen;
    long long pos = 0;
    crypto_uint8 ch;
    char *chan = (char *)b1->buf + pos;

    /* parse packet */
    pos = packetparser_uint8(b1->buf, b1->len, pos, &ch);               /* byte      SSH_MSG_CHANNEL_OPEN */
    if (ch != SSH_MSG_CHANNEL_OPEN) bug_proto();
    pos = packetparser_uint32(b1->buf, b1->len, pos, &chanlen);         /* string    channel type in US-ASCII only */
    chan = (char *)b1->buf + pos;
    pos = packetparser_skip(b1->buf, b1->len, pos, chanlen);
    pos = packetparser_uint32(b1->buf, b1->len, pos, &id);              /* uint32    sender channel */
    pos = packetparser_uint32(b1->buf, b1->len, pos, &remotewindow);    /* uint32    initial window size */
    pos = packetparser_uint32(b1->buf, b1->len, pos, &maxpacket);       /* uint32    maximum packet size */
    if (maxpacket > PACKET_LIMIT) maxpacket = PACKET_LIMIT;

    if (str_equaln(chan, chanlen, "session")) {

/*    byte      SSH_MSG_CHANNEL_OPEN
      string    "session"
      uint32    sender channel
      uint32    initial window size
      uint32    maximum packet size
*/

        pos = packetparser_end(b1->buf, b1->len, pos);

        /* send confirmation */
        buf_purge(b1);
        if (channel_open(packet.name, id, remotewindow, maxpacket, &localwindow)) {
            buf_purge(b2);
            buf_putnum8(b2, SSH_MSG_CHANNEL_OPEN_CONFIRMATION);     /* byte      SSH_MSG_CHANNEL_OPEN_CONFIRMATION */
            buf_putnum32(b2, id);                                   /* uint32    recipient channel */
            buf_putnum32(b2, id);                                   /* uint32    sender channel */
            buf_putnum32(b2, localwindow);                          /* uint32    initial window size */
            /*
            XXX
            use PACKET_LIMIT/2 as maximum packet size,
            workaround for miscalculated packet_length
            */
            buf_putnum32(b2, PACKET_LIMIT / 2);                     /* uint32    maximum packet size */
            packet_put(b2);
            buf_purge(b2);
            return 1;
        }
    }

    /* reject channel */
    buf_purge(b2);
    buf_putnum8(b2, SSH_MSG_CHANNEL_OPEN_FAILURE);              /* byte      SSH_MSG_CHANNEL_OPEN_FAILURE */
    buf_putnum32(b2, id);                                       /* uint32    recipient channel */
    buf_putnum32(b2, SSH_OPEN_ADMINISTRATIVELY_PROHIBITED);     /* uint32    reason code */
    buf_putstring(b2, "only one 'session' channel allowed");    /* string    description in ISO-10646 UTF-8 encoding [RFC3629] */
    buf_putstring(b2, "");                                      /* string    language tag [RFC3066] */
    packet_put(b2);
    buf_purge(b1); buf_purge(b2);
    return 1;
}
