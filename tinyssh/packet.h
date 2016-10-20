/*
20140108
Jan Mojzis
Public domain.
*/

#ifndef _PACKET_H____
#define _PACKET_H____

#include "buf.h"
#include "crypto_uint32.h"
#include "crypto_uint8.h"
#include "sshcrypto.h"
#include "limit.h"
#include "channel.h"

#define PACKET_UNAUTHENTICATED_MESSAGES 30
#define PACKET_LIMIT 32768
#define PACKET_FULLLIMIT 35000

#define PACKET_RECVLIMIT 131072
#define PACKET_ZEROBYTES 64

struct packet {

    /* flags */
    int flagkeys;
    int flagauthorized;
    int flagrekeying;

    /* channel */
    int flageofsent;
    int flagclosesent;
    int flagchanneleofreceived;

    /* packet id */
    crypto_uint32 sendpacketid;
    crypto_uint32 receivepacketid;

    /* keys */
    unsigned char serverkey[sshcrypto_cipher_KEYMAX];
    unsigned char clientkey[sshcrypto_cipher_KEYMAX];
    unsigned char servermackey[sshcrypto_cipher_KEYMAX];
    unsigned char clientmackey[sshcrypto_cipher_KEYMAX];
    unsigned char servernonce[sshcrypto_cipher_KEYMAX];
    unsigned char clientnonce[sshcrypto_cipher_KEYMAX];
    unsigned char sessionid[sshcrypto_hash_MAX];
    char name[LOGIN_NAME_MAX + 1];
    crypto_uint8 kex_packet_follows;
    crypto_uint8 kex_guess;

    /* buffers */
    unsigned char hellosendspace[256];
    unsigned char helloreceivespace[256];
    unsigned char kexsendspace[1024];
    unsigned char kexrecvspace[65536];
    unsigned char hashbufspace[65536];
    struct buf hellosend;
    struct buf helloreceive;
    struct buf kexsend;
    struct buf kexrecv;
    struct buf hashbuf;

    /* send/recv */
    unsigned char recvbufspace[4 * PACKET_FULLLIMIT + 1 + PACKET_ZEROBYTES];
    unsigned char sendbufspace[4 * PACKET_FULLLIMIT + 1];
    struct buf recvbuf;
    struct buf sendbuf;
    crypto_uint32 packet_length;

};

/* packet.c */
extern struct packet packet;
extern void packet_purge(void);
extern void packet_init(void);

/* packet_send.c */
extern int packet_sendisready(void);
extern int packet_send(void);
extern int packet_sendall(void);

/* packet_recv.c */
extern int packet_recvisready(void);
extern int packet_recv(void);

/* packet_get.c */
extern int packet_get(struct buf *, crypto_uint8);
extern int packet_getall(struct buf *, crypto_uint8);

/* packet_put.c */
extern void packet_put(struct buf *);
extern int packet_putisready(void);

/* packet_hello.c */
extern int packet_hello_send(void);
extern int packet_hello_receive(void);

/* packet_kex.c */
extern int packet_kex_send(void);
extern int packet_kex_receive(void);

/* packet_kexdh.c */
extern int packet_kexdh(const char *, struct buf *, struct buf *);

/* packet_auth.c */
extern int packet_auth(struct buf *, struct buf *);

/* packet_channel_open.c */
extern int packet_channel_open(struct buf *, struct buf *);

/* packet_channel_request.c */
extern int packet_channel_request(struct buf *, struct buf *);

/* packet_channel_recv.c */
extern int packet_channel_recv_data(struct buf *);
extern int packet_channel_recv_extendeddata(struct buf *);
extern int packet_channel_recv_windowadjust(struct buf *);
extern int packet_channel_recv_eof(struct buf *);
extern int packet_channel_recv_close(struct buf *);

/* packet_channel_send.c */
extern void packet_channel_send_data(struct buf *);
extern void packet_channel_send_extendeddata(struct buf *);
extern int packet_channel_send_windowadjust(struct buf *);
extern void packet_channel_send_eof(struct buf *);
extern int packet_channel_send_close(struct buf *, int, int);

/* packet_unimplemented.c */
extern int packet_unimplemented(struct buf *);

#endif
