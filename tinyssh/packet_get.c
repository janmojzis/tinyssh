/*
20140126
Jan Mojzis
Public domain.
*/

#include <poll.h>
#include <unistd.h>
#include "byte.h"
#include "e.h"
#include "bug.h"
#include "uint32_unpack_big.h"
#include "purge.h"
#include "ssh.h"
#include "sshcrypto.h"
#include "numtostr.h"
#include "packet.h"

static int packet_get_plain_(struct buf *b) {

    crypto_uint32 packet_length;
    long long len;
    struct buf *recvbuf = &packet.recvbuf;
    unsigned char *pp;
    long long l;

    pp = recvbuf->buf + PACKET_ZEROBYTES;
    l  = recvbuf->len - PACKET_ZEROBYTES;

    /* we need at least 4 bytes */
    if (l < 4) return 1;

    /* parse length */
    packet_length = uint32_unpack_big(pp);
    if (packet_length > PACKET_LIMIT) {
        char buf1[NUMTOSTR_LEN];
        char buf2[NUMTOSTR_LEN];
        errno = EPROTO;
        log_f4("packet length ", numtostr(buf1, packet_length), " > PACKET_LIMIT ", numtostr(buf2, PACKET_LIMIT));
        global_die(111);
    }
    if (packet_length + 4 > l) return 1;

    /* we have full packet */
    len = packet_length;
    len -= recvbuf->buf[PACKET_ZEROBYTES + 4] + 1;
    if (len <= 0) bug_proto();
    buf_put(b, recvbuf->buf + PACKET_ZEROBYTES + 5, len);

    byte_copy(pp,  l - packet_length + 4, pp + packet_length + 4);
    purge(pp + l - packet_length + 4, packet_length + 4);
    recvbuf->len -= packet_length + 4;

    packet.receivepacketid++;
    return 1;
}

static int packet_get_(struct buf *b) {

    if (packet.flagkeys) {
        return sshcrypto_packet_get(b);
    }
    else {
        return packet_get_plain_(b);
    }
}


int packet_get(struct buf *b, crypto_uint8 x) {

    buf_purge(b);
    if (!packet_get_(b)) return 0;
    if (b->len <= 0) return 1;
    if (!packet.flagauthorized) if (packet.receivepacketid > PACKET_UNAUTHENTICATED_MESSAGES) {
        errno = EPROTO;
        log_f1("too many unauthenticated messages");
        global_die(111);
    }

    switch (b->buf[0]) {
        case SSH_MSG_DISCONNECT:
            errno = 0;
            return 0;
        case SSH_MSG_IGNORE:
        case SSH_MSG_DEBUG:
            buf_purge(b);
            break;
        default:
            if (x && x != b->buf[0]) {
                char buf1[NUMTOSTR_LEN];
                char buf2[NUMTOSTR_LEN];
                errno = EPROTO;
                log_f4("expected packet type ", numtostr(buf1, x), ", got ", numtostr(buf2, b->buf[0]));
                global_die(111);
            }
            break;
    }
    return 1;
}

int packet_getall(struct buf *b, crypto_uint8 ch) {

    struct pollfd x;
    long long before;

    buf_purge(b);

    for (;;) {
        before = packet.recvbuf.len;
        if (!packet_get(b, ch)) return 0;
        if (b->len > 0) break;
        if (before != packet.recvbuf.len) continue;
        x.fd = 0;
        x.events = POLLIN | POLLERR;
        poll(&x, 1, -1);
        if (!packet_recv()) return 0;
    }
    return 1;
}
