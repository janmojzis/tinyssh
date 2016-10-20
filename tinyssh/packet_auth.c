/*
20140110
Jan Mojzis
Public domain.
*/

#include "buf.h"
#include "ssh.h"
#include "e.h"
#include "str.h"
#include "packetparser.h"
#include "subprocess.h"
#include "sshcrypto.h"
#include "bug.h"
#include "purge.h"
#include "log.h"
#include "packet.h"

int packet_auth(struct buf *b, struct buf *b2) {

    crypto_uint8 ch, flagsignature;
    long long pos, i, count, sign_bytes = 0;
    crypto_uint32 len;
    const char *pkname;
    int (*sign_open)(unsigned char *,unsigned long long *,const unsigned char *,unsigned long long,const unsigned char *) = 0;
    int (*parsesignpk)(unsigned char *, const unsigned char *, long long) = 0;
    int (*parsesignature)(unsigned char *, const unsigned char *, long long) = 0;
    void (*putsignpk)(struct buf *, const unsigned char *) = 0;
    void (*putsignpkbase64)(struct buf *, const unsigned char *) = 0;
    unsigned char pk[sshcrypto_sign_PUBLICKEYMAX];
    unsigned char sig[sshcrypto_sign_MAX];
    unsigned long long smlen;

    buf_purge(b);

    /* parse "ssh-userauth" */
    pos = 0;
    if (!packet_getall(b, SSH_MSG_SERVICE_REQUEST)) bug();
    pos = packetparser_uint8(b->buf, b->len, pos, &ch);       /* SSH_MSG_SERVICE_REQUEST */
    if (ch != SSH_MSG_SERVICE_REQUEST) bug_proto();
    pos = packetparser_uint32(b->buf, b->len, pos, &len);     /* "ssh-userauth" */
    pos = packetparser_skip(b->buf, b->len, pos, len);
    if (!str_equaln((char *)b->buf + pos - len, len, "ssh-userauth")) bug_proto();
    pos = packetparser_end(b->buf, b->len, pos);

    /* send service accept */
    b->buf[0] = SSH_MSG_SERVICE_ACCEPT;
    packet_put(b);
    if (!packet_sendall()) bug();


    for (count = 0; count < 32; ++count) {
        /* receive userauth request */
        pkname = "unknown";
        pos = 0;
        buf_purge(b);
        if (!packet_getall(b, SSH_MSG_USERAUTH_REQUEST)) bug();
        pos = packetparser_uint8(b->buf, b->len, pos, &ch);         /* SSH_MSG_USERAUTH_REQUEST */
        if (ch != SSH_MSG_USERAUTH_REQUEST) bug_proto();
        pos = packetparser_uint32(b->buf, b->len, pos, &len);       /* name */
        if (len >= sizeof packet.name) bug_proto();
        pos = packetparser_copy(b->buf, b->len, pos, (unsigned char *)packet.name, len);
        packet.name[len] = 0;
        pos = packetparser_uint32(b->buf, b->len, pos, &len);       /* "ssh-connection" */
        pos = packetparser_skip(b->buf, b->len, pos, len);
        if (!str_equaln((char *)b->buf + pos - len, len, "ssh-connection")) bug_proto();

        pos = packetparser_uint32(b->buf, b->len, pos, &len);       /* publickey/password/hostbased/none */
        pos = packetparser_skip(b->buf, b->len, pos, len);

        if (str_equaln((char *)b->buf + pos - len, len, "none")) pkname = "none";
        if (str_equaln((char *)b->buf + pos - len, len, "password")) pkname = "password";
        if (str_equaln((char *)b->buf + pos - len, len, "hostbased")) pkname = "hostbased";
        if (str_equaln((char *)b->buf + pos - len, len, "publickey")) {
            pos = packetparser_uint8(b->buf, b->len, pos, &flagsignature);

            pos = packetparser_uint32(b->buf, b->len, pos, &len);   /* public key algorithm name */
            pos = packetparser_skip(b->buf, b->len, pos, len);
            if (b->buf[pos] != 0) bug_proto();
            pkname = (char *)b->buf + pos - len; /* XXX */

            sign_open = 0; parsesignpk = 0; putsignpk = 0; putsignpkbase64 = 0; parsesignature = 0; sign_bytes = 0;
            for (i = 0; sshcrypto_keys[i].name; ++i) {
                if (!sshcrypto_keys[i].sign_flagclient) continue;
                if (!str_equaln(pkname, len, sshcrypto_keys[i].name)) continue;
                pkname = sshcrypto_keys[i].name;
                sign_open = sshcrypto_keys[i].sign_open;
                parsesignature = sshcrypto_keys[i].parsesignature;
                parsesignpk = sshcrypto_keys[i].parsesignpk;
                putsignpk = sshcrypto_keys[i].buf_putsignpk;
                putsignpkbase64 = sshcrypto_keys[i].buf_putsignpkbase64;
                sign_bytes = sshcrypto_keys[i].sign_bytes;
                break;
            }
            if (sign_open && parsesignpk && putsignpk && putsignpkbase64 && parsesignature) {
                pos = packetparser_uint32(b->buf, b->len, pos, &len);   /* public key blob */
                pos = packetparser_skip(b->buf, b->len, pos, len);
                if (!parsesignpk(pk, b->buf + pos - len, len)) bug_proto();

                if (!flagsignature) {
                    /* 'publickey' ... without signature */
                    buf_purge(b);
                    buf_putnum8(b, SSH_MSG_USERAUTH_PK_OK);
                    buf_putstring(b, pkname);
                    putsignpk(b, pk);
                    packet_put(b);
                    if (!packet_sendall()) bug();
                    continue;
                }


                /* 'publickey' ... with signature */
                pos = packetparser_uint32(b->buf, b->len, pos, &len);   /* signature blob */
                pos = packetparser_skip(b->buf, b->len, pos, len);
                if (!parsesignature(sig, b->buf + pos - len, len)) bug_proto();
                pos = packetparser_end(b->buf, b->len, pos);
                purge(b->buf + b->len - len - 4, len + 4);
                b->len -= len + 4;


                /* authenticate user - verify signature */
                buf_purge(b2);
                buf_put(b2, sig, sign_bytes);
                buf_putstringlen(b2, packet.sessionid, sshcrypto_hash_bytes);
                buf_put(b2, b->buf, b->len);

                buf_purge(b);
                if (b->alloc <= b2->len) bug_nomem();
                if (sign_open(b->buf, &smlen, b2->buf, b2->len, pk) != 0) { errno = EAUTH; bug(); }
                b->len = smlen; buf_purge(b);

                /* authorize user -  using authorized_keys */
                buf_purge(b);
                putsignpkbase64(b, pk);
                buf_putnum8(b, 0);
                if (subprocess_auth(packet.name, pkname, (char *)b->buf) == 0) goto authorized;
            }
        }

        /* reject */
        log_d5("auth: ", packet.name, ": ", pkname, " rejected");
        buf_purge(b);
        buf_putnum8(b, SSH_MSG_USERAUTH_FAILURE);
        buf_putstring(b,"publickey");
        buf_putnum8(b, 0);
        packet_put(b);
        if (!packet_sendall()) bug();
    }
    log_w1("auth: too many authentication tries");
    return 0;


authorized:
    /* authenticated + authorized */
    log_i7("auth: ", packet.name, ": ", pkname, " ", (char *)b->buf, " accepted");
    buf_purge(b);
    buf_putnum8(b, SSH_MSG_USERAUTH_SUCCESS);
    buf_putstring(b,"ssh-connection");
    packet_put(b);
    if (!packet_sendall()) bug();

    purge(pk, sizeof pk);
    purge(sig, sizeof sig);
    return 1;
}
