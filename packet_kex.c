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
#include "packet.h"

int packet_kex_send(void) {

    struct buf *b = &packet.kexsend;

    /* send server kex_init */
    buf_purge(b);
    buf_putnum8(b, SSH_MSG_KEXINIT);       /* SSH_MSG_KEXINIT */
    buf_putrandombytes(b, 16);             /* cookie */
    sshcrypto_kex_put(b);                  /* kex algorithms */
    sshcrypto_key_put(b);                  /* server host key algorithms */
    sshcrypto_cipher_put(b);               /* encryption algorithms client to server */
    sshcrypto_cipher_put(b);               /* encryption algorithms server to client */
    sshcrypto_cipher_macput(b);            /* mac algorithms client to server */
    sshcrypto_cipher_macput(b);            /* mac algorithms server to client */
    buf_putstring(b, "none");              /* compress algorithms client to server */
    buf_putstring(b, "none");              /* compress algorithms server to client */
    buf_putstring(b, "");                  /* languages client to server */
    buf_putstring(b, "");                  /* languages server to client */
    buf_putnum8(b, 0);                     /* kex first packet follows  */
    buf_putnum32(b, 0);                    /* reserved */

    packet_put(b);
    return packet_sendall();
}

int packet_kex_receive(void) {

    struct buf *b = &packet.kexrecv;
    long long pos = 0;
    crypto_uint8 ch;
    crypto_uint32 len;

    if (!packet_getall(b, SSH_MSG_KEXINIT)) return 0;

    /* parse packet */
    pos = packetparser_uint8(b->buf, b->len, pos, &ch);       /* SSH_MSG_KEXINIT */
    if (ch != SSH_MSG_KEXINIT) bug_proto();

    pos = packetparser_skip(b->buf, b->len, pos, 16);         /* cookie */

    pos = packetparser_uint32(b->buf, b->len, pos, &len);     /* kex algorithms */
    pos = packetparser_skip(b->buf, b->len, pos, len);        
    if (!sshcrypto_kex_select(b->buf + pos - len, len, &packet.kex_guess)) return 0;

    pos = packetparser_uint32(b->buf, b->len, pos, &len);     /* server host key algorithms */
    pos = packetparser_skip(b->buf, b->len, pos, len);        
    if (!sshcrypto_key_select(b->buf + pos - len, len)) return 0;

    pos = packetparser_uint32(b->buf, b->len, pos, &len);     /* encryption algorithms client to server */
    pos = packetparser_skip(b->buf, b->len, pos, len);        
    if (!sshcrypto_cipher_select(b->buf + pos - len, len)) return 0;

    pos = packetparser_uint32(b->buf, b->len, pos, &len);     /* encryption algorithms server to client */
    pos = packetparser_skip(b->buf, b->len, pos, len);        
    /* XXX assuming same as encryption algorithms client to server  */

    pos = packetparser_uint32(b->buf, b->len, pos, &len);     /* mac algorithms client to server */
    pos = packetparser_skip(b->buf, b->len, pos, len);        
    if (!sshcrypto_cipher_macselect(b->buf + pos - len, len)) return 0;

    pos = packetparser_uint32(b->buf, b->len, pos, &len);     /* mac algorithms server to client */
    pos = packetparser_skip(b->buf, b->len, pos, len);        

    pos = packetparser_uint32(b->buf, b->len, pos, &len);     /* compress algorithms client to server */
    pos = packetparser_skip(b->buf, b->len, pos, len);        

    pos = packetparser_uint32(b->buf, b->len, pos, &len);     /* compress algorithms server to client */
    pos = packetparser_skip(b->buf, b->len, pos, len);        

    pos = packetparser_uint32(b->buf, b->len, pos, &len);     /* languages client to server */
    pos = packetparser_skip(b->buf, b->len, pos, len);        

    pos = packetparser_uint32(b->buf, b->len, pos, &len);     /* languages server to client */
    pos = packetparser_skip(b->buf, b->len, pos, len);        

    pos = packetparser_uint8(b->buf, b->len, pos, &ch);       /* kex first packet follows */
    packet.kex_packet_follows = ch;

    pos = packetparser_uint32(b->buf, b->len, pos, &len);     /* reserved */
    pos = packetparser_end(b->buf, b->len, pos);

    return 1;
}
