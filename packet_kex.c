/*
20140110
Jan Mojzis
Public domain.
*/

#include "buf.h"
#include "ssh.h"
#include "sshcrypto.h"
#include "packetparser.h"
#include "bug.h"
#include "log.h"
#include "packet.h"
#include "str.h"
#include "stringparser.h"

static int namelist_contains(const unsigned char *buf, long long len,
                             const char *name) {
    const unsigned char *x;
    long long pos = 0;
    long long xlen;

    for (;;) {
        pos = stringparser(buf, len, pos, &x, &xlen);
        if (!pos) break;
        if (str_equaln((const char *) x, xlen, name)) return 1;
    }
    return 0;
}

int packet_kex_send(void) {

    struct buf *b = &packet.kexsend;

    /* send server kex_init */
    buf_purge(b);
    buf_putnum8(b, SSH_MSG_KEXINIT); /* SSH_MSG_KEXINIT */
    buf_putrandombytes(b, 16);       /* cookie */
    sshcrypto_kex_put(b);            /* kex algorithms */
    sshcrypto_key_put(b);            /* server host key algorithms */
    sshcrypto_cipher_put(b);    /* encryption algorithms client to server */
    sshcrypto_cipher_put(b);    /* encryption algorithms server to client */
    sshcrypto_cipher_macput(b); /* mac algorithms client to server */
    sshcrypto_cipher_macput(b); /* mac algorithms server to client */
    buf_putstring(b, "none");   /* compress algorithms client to server */
    buf_putstring(b, "none");   /* compress algorithms server to client */
    buf_putstring(b, "");       /* languages client to server */
    buf_putstring(b, "");       /* languages server to client */
    buf_putnum8(b, 0);          /* kex first packet follows  */
    buf_putnum32(b, 0);         /* reserved */

    packet_put(b);
    return packet_sendall();
}

static int packet_kex_parse(int initial) {

    struct buf *b = &packet.kexrecv;
    long long pos = 0;
    crypto_uint8 ch;
    crypto_uint32 len;

    /* parse packet */
    pos = packetparser_uint8(b->buf, b->len, pos, &ch); /* SSH_MSG_KEXINIT */
    if (ch != SSH_MSG_KEXINIT) bug_proto();

    pos = packetparser_skip(b->buf, b->len, pos, 16); /* cookie */

    pos = packetparser_uint32(b->buf, b->len, pos, &len); /* kex algorithms */
    pos = packetparser_skip(b->buf, b->len, pos, len);
    if (!sshcrypto_kex_select(b->buf + pos - len, len, &packet.kex_guess,
                              initial))
        return 0;
    if (initial && sshcrypto_kex_flags & sshcrypto_FLAGSTRICTKEX)
        if (packet.receivepacketid != 1) {
            log_f1("strict KEX mode: SSH_MSG_KEXINIT was not the first packet");
            return 0;
        }

    pos = packetparser_uint32(b->buf, b->len, pos,
                              &len); /* server host key algorithms */
    pos = packetparser_skip(b->buf, b->len, pos, len);
    if (!sshcrypto_key_select(b->buf + pos - len, len)) return 0;

    pos = packetparser_uint32(
        b->buf, b->len, pos, &len); /* encryption algorithms client to server */
    pos = packetparser_skip(b->buf, b->len, pos, len);
    if (!sshcrypto_cipher_select(b->buf + pos - len, len)) return 0;

    pos = packetparser_uint32(
        b->buf, b->len, pos, &len); /* encryption algorithms server to client */
    pos = packetparser_skip(b->buf, b->len, pos, len);
    if (!namelist_contains(b->buf + pos - len, len, sshcrypto_cipher_name)) {
        errno = EPROTO;
        return 0;
    }

    pos = packetparser_uint32(b->buf, b->len, pos,
                              &len); /* mac algorithms client to server */
    pos = packetparser_skip(b->buf, b->len, pos, len);
    if (!sshcrypto_cipher_macselect(b->buf + pos - len, len)) return 0;

    pos = packetparser_uint32(b->buf, b->len, pos,
                              &len); /* mac algorithms server to client */
    pos = packetparser_skip(b->buf, b->len, pos, len);

    pos = packetparser_uint32(b->buf, b->len, pos,
                              &len); /* compress algorithms client to server */
    pos = packetparser_skip(b->buf, b->len, pos, len);
    if (!namelist_contains(b->buf + pos - len, len, "none")) {
        errno = EPROTO;
        return 0;
    }

    pos = packetparser_uint32(b->buf, b->len, pos,
                              &len); /* compress algorithms server to client */
    pos = packetparser_skip(b->buf, b->len, pos, len);
    if (!namelist_contains(b->buf + pos - len, len, "none")) {
        errno = EPROTO;
        return 0;
    }

    pos = packetparser_uint32(b->buf, b->len, pos,
                              &len); /* languages client to server */
    pos = packetparser_skip(b->buf, b->len, pos, len);

    pos = packetparser_uint32(b->buf, b->len, pos,
                              &len); /* languages server to client */
    pos = packetparser_skip(b->buf, b->len, pos, len);

    pos = packetparser_uint8(b->buf, b->len, pos,
                             &ch); /* kex first packet follows */
    packet.kex_packet_follows = ch;

    pos = packetparser_uint32(b->buf, b->len, pos, &len); /* reserved */
    pos = packetparser_end(b->buf, b->len, pos);

    return 1;
}

int packet_kex_receive(void) {

    if (!packet_getall(&packet.kexrecv, SSH_MSG_KEXINIT)) return 0;
    return packet_kex_parse(1);
}

int packet_kex_receive_rekey(struct buf *b) {

    if (!b || b == &packet.kexrecv) bug_inval();
    buf_purge(&packet.kexrecv);
    buf_put(&packet.kexrecv, b->buf, b->len);
    return packet_kex_parse(0);
}
