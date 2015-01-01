/*
20140211
Jan Mojzis
Public domain.
*/

#include "bug.h"
#include "buf.h"
#include "channel.h"
#include "ssh.h"
#include "bug.h"
#include "uint32_pack_big.h"
#include "packet.h"

void packet_channel_send_data(struct buf *b) {

    long long r;

    buf_purge(b);

    if (b->alloc <= PACKET_LIMIT) bug_nomem();
    if (!packet_putisready()) return;
    r = channel_read(b->buf + 9, PACKET_LIMIT - 9);
    if (r == 0) return;
    b->len = r + 9;
    b->buf[0] = SSH_MSG_CHANNEL_DATA;                   /* byte      SSH_MSG_CHANNEL_DATA */
    uint32_pack_big(b->buf + 1, channel_getid());       /* uint32    recipient channel    */
    uint32_pack_big(b->buf + 5, r);                     /* string    data                 */
    packet_put(b);
    buf_purge(b);
    return;
}

void packet_channel_send_extendeddata(struct buf *b) {

    long long r;

    buf_purge(b);

    if (b->alloc <= PACKET_LIMIT) bug_nomem();
    if (!packet_putisready()) return;
    r = channel_extendedread(b->buf + 13, PACKET_LIMIT - 13);
    if (r == 0) return;
    b->len = r + 13;
    b->buf[0] = SSH_MSG_CHANNEL_EXTENDED_DATA;          /* byte      SSH_MSG_CHANNEL_EXTENDED_DATA */
    uint32_pack_big(b->buf + 1, channel_getid());       /* uint32    recipient channel             */
    uint32_pack_big(b->buf + 5, 1);                     /* uint32    data_type_code (1 = stderr)   */
    uint32_pack_big(b->buf + 9, r);                     /* string    data                          */
    packet_put(b);
    buf_purge(b);
    return;
}


int packet_channel_send_windowadjust(struct buf *b) {

    crypto_uint32 plus;

    if (channel_getlen0() > CHANNEL_BUFSIZE / 2) return 1;
    if (channel_getlocalwindow() > CHANNEL_BUFSIZE / 2) return 1;

    buf_purge(b);
    buf_putnum8(b, SSH_MSG_CHANNEL_WINDOW_ADJUST);
    buf_putnum32(b, channel_getid());
    plus = CHANNEL_BUFSIZE - channel_getlen0() - channel_getlocalwindow();
    buf_putnum32(b, plus);
    channel_incrementlocalwindow(plus);
    packet_put(b);
    buf_purge(b);
    return 1;
}


void packet_channel_send_eof(struct buf *b) {

    if (packet.flageofsent) return;

    buf_purge(b);
    buf_putnum8(b, SSH_MSG_CHANNEL_EOF);
    buf_putnum32(b, channel_getid());
    packet_put(b);
    buf_purge(b);
    packet.flageofsent = 1;
}

int packet_channel_send_close(struct buf *b, int exitsignal, int exitcode) {

    packet_channel_send_eof(b);
    if (packet.flagclosesent) return 1;

    if (exitsignal > 0) {
        buf_purge(b);
        buf_putnum8(b, SSH_MSG_CHANNEL_REQUEST);        /* byte      SSH_MSG_CHANNEL_REQUEST */
        buf_putnum32(b, channel_getid());               /* uint32    recipient channel */
        buf_putstring(b, "exit-signal");                /* string    "exit-signal" */
        buf_putnum8(b, 0);                              /* boolean   FALSE */
        buf_putstring(b, ssh_sigstr(exitsignal));       /* string    signal name (without the "SIG" prefix) */
        buf_putnum8(b, 0);                              /* boolean   core dumped */
        buf_putstring(b, ssh_sigstrlong(exitsignal));   /* string    error message in ISO-10646 UTF-8 encoding */
        buf_putstring(b, "");                           /* string    language tag [RFC3066] */
        packet_put(b);
        buf_purge(b);
        if (!packet_sendall()) return 0;
    }
    else {
        buf_purge(b);
        buf_putnum8(b, SSH_MSG_CHANNEL_REQUEST);        /* byte      SSH_MSG_CHANNEL_REQUEST */
        buf_putnum32(b, channel_getid());               /* uint32    recipient channel */
        buf_putstring(b, "exit-status");                /* string    "exit-status" */
        buf_putnum8(b, 0);                              /* boolean   FALSE */
        buf_putnum32(b, exitcode);                      /* uint32    exit_status */
        packet_put(b);
        buf_purge(b);
        if (!packet_sendall()) return 0;
    }

    buf_purge(b);
    buf_putnum8(b, SSH_MSG_CHANNEL_CLOSE);              /* byte      SSH_MSG_CHANNEL_CLOSE */
    buf_putnum32(b, channel_getid());                   /* uint32    recipient channel */
    packet_put(b);
    if (!packet_sendall()) return 0;
    buf_purge(b);

    packet.flagclosesent = 1;
    return 1;
}
