/*
20141025
Jan Mojzis
Public domain.
*/

#include "buf.h"
#include "ssh.h"
#include "e.h"
#include "uint32_unpack_big.h"
#include "byte.h"
#include "sshcrypto.h"
#include "packetparser.h"
#include "bug.h"
#include "log.h"
#include "purge.h"
#include "subprocess.h"
#include "packet.h"


int packet_kexdh(const char *keydir, struct buf *b1, struct buf *b2) {

    unsigned char clientpk[sshcrypto_dh_PUBLICKEYMAX];
    unsigned char serversk[sshcrypto_dh_SECRETKEYMAX];
    unsigned char serverpk[sshcrypto_dh_PUBLICKEYMAX];
    unsigned char sharedsecret[sshcrypto_dh_MAX];
    unsigned char sm[sshcrypto_sign_MAX];
    unsigned char key[sshcrypto_cipher_KEYMAX];
    unsigned char hash[sshcrypto_hash_MAX];
    long long pos = 0;
    crypto_uint8 ch;
    crypto_uint32 len;
    long long i;

    if (packet.kex_packet_follows && !packet.kex_guess) {
        buf_purge(b1);
        if (!packet_getall(b1, SSH_MSG_KEXDH_INIT)) bug_proto();
    }

    buf_purge(b1);
    if (!packet_getall(b1, SSH_MSG_KEXDH_INIT)) bug_proto();
    pos = packetparser_uint8(b1->buf, b1->len, pos, &ch);           /* byte      SSH_MSG_KEXDH_INIT */
    if (ch != SSH_MSG_KEXDH_INIT) bug_proto();
    pos = packetparser_uint32(b1->buf, b1->len, pos, &len);         /* string    client's public key */
    if (len != sshcrypto_dh_publickeybytes) bug_proto();
    pos = packetparser_copy(b1->buf, b1->len, pos, clientpk, len);
    pos = packetparser_end(b1->buf, b1->len, pos);
    buf_purge(b1);

    /* generate key and compute shared secret */
    do { 
        /* XXX - workaroud for bug in OpenSSH 6.5 - 6.6 */
        if (sshcrypto_dh_keypair(serverpk, serversk) != 0) bug_proto();
        if (sshcrypto_dh(sharedsecret, clientpk, serversk) != 0) bug_proto();
    } while(sharedsecret[0] == 0 && sshcrypto_dh_publickeybytes == 32);

    /* create hash */
    buf_purge(&packet.hashbuf);
    buf_putstringlen(&packet.hashbuf, packet.helloreceive.buf, packet.helloreceive.len);
    buf_putstringlen(&packet.hashbuf, packet.hellosend.buf, packet.hellosend.len);
    buf_putstringlen(&packet.hashbuf, packet.kexrecv.buf, packet.kexrecv.len);
    buf_putstringlen(&packet.hashbuf, packet.kexsend.buf, packet.kexsend.len);
    sshcrypto_buf_putsignpk(&packet.hashbuf, sshcrypto_sign_publickey);
    sshcrypto_buf_putdhpk(&packet.hashbuf, clientpk);
    sshcrypto_buf_putdhpk(&packet.hashbuf, serverpk);
    sshcrypto_buf_putsharedsecret(&packet.hashbuf, sharedsecret);
    sshcrypto_hash(hash, packet.hashbuf.buf, packet.hashbuf.len);

    /* session id */
    if (!packet.flagrekeying) byte_copy(packet.sessionid, sshcrypto_hash_bytes, hash);
    packet.flagrekeying = 1;

    /* signature */
    if (subprocess_sign(sm, sshcrypto_sign_bytes, keydir, hash, sshcrypto_hash_bytes) != 0) bug();
    buf_purge(b1); buf_purge(b2);

    /* send server kex_ecdh_reply */
    buf_putnum8(b2, SSH_MSG_KEXDH_REPLY);                                  /* SSH_MSG_KEXDH_REPLY */
    sshcrypto_buf_putsignpk(b2, sshcrypto_sign_publickey);                 /* public key */
    sshcrypto_buf_putdhpk(b2, serverpk);                                   /* servers's public key */
    sshcrypto_buf_putsignature(b2, sm);                                    /* signature */
    packet_put(b2);

    /* send server newkeys */
    buf_purge(b2);
    buf_putnum8(b2, SSH_MSG_NEWKEYS);
    packet_put(b2);
    if (!packet_sendall()) bug();

    /* receive new keys */
    do {
        buf_purge(b2);
        if (!packet_getall(b2, 0)) bug();
    } while (b2->buf[0] != SSH_MSG_NEWKEYS);

    /* key derivation */
    for(i = 0; i < 6; ++i) {
        buf_purge(b1);
        sshcrypto_buf_putsharedsecret(b1, sharedsecret);
        buf_put(b1, hash, sshcrypto_hash_bytes);
        buf_putnum8(b1, 'A' + i);
        buf_put(b1, packet.sessionid, sshcrypto_hash_bytes);
        sshcrypto_hash(key, b1->buf, b1->len);

        /* one extend */
        buf_purge(b1);
        sshcrypto_buf_putsharedsecret(b1, sharedsecret);
        buf_put(b1, hash, sshcrypto_hash_bytes);
        buf_put(b1, key, sshcrypto_hash_bytes);
        sshcrypto_hash(key + sshcrypto_hash_bytes, b1->buf, b1->len);

        if (i == 0) byte_copy(packet.clientnonce, sshcrypto_cipher_KEYMAX, key);
        if (i == 1) byte_copy(packet.servernonce, sshcrypto_cipher_KEYMAX, key);
        if (i == 2) byte_copy(packet.clientkey, sshcrypto_cipher_KEYMAX, key);
        if (i == 3) byte_copy(packet.serverkey, sshcrypto_cipher_KEYMAX, key);
        if (i == 4) byte_copy(packet.clientmackey, sshcrypto_cipher_KEYMAX, key);
        if (i == 5) byte_copy(packet.servermackey, sshcrypto_cipher_KEYMAX, key);
    }

    purge(clientpk, sizeof clientpk);
    purge(serversk, sizeof serversk);
    purge(serverpk, sizeof serverpk);
    purge(sharedsecret, sizeof sharedsecret);
    purge(sm, sizeof sm);
    purge(key, sizeof key);
    purge(hash, sizeof hash);
    return 1;
}
