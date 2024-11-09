/*
20140120
Jan Mojzis
Public domain.
*/

#include "buf.h"
#include "ssh.h"
#include "e.h"
#include "bug.h"
#include "str.h"
#include "log.h"
#include "packetparser.h"
#include "packet.h"

int packet_channel_request(struct buf *b1, struct buf *b2, const char *customcmd) {

    long long pos = 0;
    crypto_uint8 ch, wantreply;
    crypto_uint32 id, a, b, x, y;
    char *cmd;
    crypto_uint32 cmdlen;
    char *p1, *p2;
    crypto_uint32 plen1, plen2;

    pos = packetparser_uint8(b1->buf, b1->len, pos, &ch);             /* byte      SSH_MSG_CHANNEL_REQUEST */
    if (ch != SSH_MSG_CHANNEL_REQUEST) bug_proto();
    pos = packetparser_uint32(b1->buf, b1->len, pos, &id);            /* uint32    recipient channel */
    if (id != channel_getid()) bug_proto();
    pos = packetparser_uint32(b1->buf, b1->len, pos, &cmdlen);        /* string    request type in US-ASCII characters only */
    cmd = (char *)b1->buf + pos;
    pos = packetparser_skip(b1->buf, b1->len, pos, cmdlen);
    pos = packetparser_uint8(b1->buf, b1->len, pos, &wantreply);      /* boolean   want reply */
    cmd[cmdlen] = 0;

    if (str_equaln(cmd, cmdlen, "exec")) {

/*      byte      SSH_MSG_CHANNEL_REQUEST
        uint32    recipient channel
        string    "exec"
        boolean   want reply
        string    command
*/
        pos = packetparser_uint32(b1->buf, b1->len, pos, &plen1);
        p1 = (char *)b1->buf + pos;
        pos = packetparser_skip(b1->buf, b1->len, pos, plen1);
        pos = packetparser_end(b1->buf, b1->len, pos);
        buf_putnum8(b1, 0);
        p1[plen1] = 0;

        if (customcmd) {
            log_d4("packet=SSH_MSG_CHANNEL_REQUEST, exec ", p1, ", rejected: custom program is selected using param. -e ", customcmd);
            goto reject;
        }

        if (!channel_exec(p1)) bug();
        log_d3("packet=SSH_MSG_CHANNEL_REQUEST, exec ", p1, ", accepted");
        goto accept;
    }

    if (str_equaln(cmd, cmdlen, "subsystem")) {

/*      byte      SSH_MSG_CHANNEL_REQUEST
        uint32    recipient channel
        string    "subsystem"
        boolean   want reply
        string    subsystem name
*/
        pos = packetparser_uint32(b1->buf, b1->len, pos, &plen1);
        p1 = (char *)b1->buf + pos;
        pos = packetparser_skip(b1->buf, b1->len, pos, plen1);
        pos = packetparser_end(b1->buf, b1->len, pos);
        buf_putnum8(b1, 0);
        p1[plen1] = 0;

        if (customcmd) {
            log_d4("packet=SSH_MSG_CHANNEL_REQUEST, subsystem ", p1, ", rejected: custom program is selected using param. -e ", customcmd);
            goto reject;
        }

        p2 = channel_subsystem_get(p1);
        if (!p2) {
            log_d3("packet=SSH_MSG_CHANNEL_REQUEST, subsystem ", p1, ", rejected");
            goto reject;
        }

        if (!channel_exec(p2)) bug();
        log_d5("packet=SSH_MSG_CHANNEL_REQUEST, subsystem ", p1, "=", p2, ", accepted");

        goto accept;
    }

    if (str_equaln(cmd, cmdlen, "shell")) {

/*      byte      SSH_MSG_CHANNEL_REQUEST
        uint32    recipient channel
        string    "shell"
        boolean   want reply
*/

        pos = packetparser_end(b1->buf, b1->len, pos);

        if (customcmd) {
            if (!channel_exec(customcmd)) bug();
            log_d3("packet=SSH_MSG_CHANNEL_REQUEST, shell, accepted, executing custom shell '", customcmd, "'");
        } 
        else {
            if (!channel_exec(0)) bug();
            log_d1("packet=SSH_MSG_CHANNEL_REQUEST, shell, accepted");
        }
        goto accept;
    }

    if (str_equaln(cmd, cmdlen, "env")) {

/*      byte      SSH_MSG_CHANNEL_REQUEST
        uint32    recipient channel
        string    "env"
        boolean   want reply
        string    variable name
        string    variable value
*/
        /**/
        pos = packetparser_uint32(b1->buf, b1->len, pos, &plen1);     /* string    variable name */
        p1 = (char *)b1->buf + pos;
        pos = packetparser_skip(b1->buf, b1->len, pos, plen1);
        pos = packetparser_uint32(b1->buf, b1->len, pos, &plen2);     /* string    variable value */
        p2 = (char *)b1->buf + pos;
        pos = packetparser_skip(b1->buf, b1->len, pos, plen2);
        pos = packetparser_end(b1->buf, b1->len, pos);
        buf_putnum8(b1, 0);
        p1[plen1] = 0;
        p2[plen2] = 0;

        if (channel_env(p1, p2)) {
            log_d5("packet=SSH_MSG_CHANNEL_REQUEST, env ", p1, "=", p2, ", accepted");
            goto accept;
        }
        else {
            log_d5("packet=SSH_MSG_CHANNEL_REQUEST, env ", p1, "=", p2, ", rejected");
            goto reject;
        }
    }

    if (str_equaln(cmd, cmdlen, "pty-req")) {
/*
        byte      SSH_MSG_CHANNEL_REQUEST
        uint32    recipient channel
        string    "pty-req"
        boolean   want_reply
        string    TERM environment variable value (e.g., vt100)
        uint32    terminal width, characters (e.g., 80)
        uint32    terminal height, rows (e.g., 24)
        uint32    terminal width, pixels (e.g., 640)
        uint32    terminal height, pixels (e.g., 480)
        string    encoded terminal modes
*/
        pos = packetparser_uint32(b1->buf, b1->len, pos, &plen1);
        p1 = (char *)b1->buf + pos;
        pos = packetparser_skip(b1->buf, b1->len, pos, plen1);
        pos = packetparser_uint32(b1->buf, b1->len, pos, &a);
        pos = packetparser_uint32(b1->buf, b1->len, pos, &b);
        pos = packetparser_uint32(b1->buf, b1->len, pos, &x);
        pos = packetparser_uint32(b1->buf, b1->len, pos, &y);
        pos = packetparser_uint32(b1->buf, b1->len, pos, &plen2);
        p2 = (char *)b1->buf + pos;
        pos = packetparser_skip(b1->buf, b1->len, pos, plen2);
        pos = packetparser_end(b1->buf, b1->len, pos);
        buf_putnum8(b1, 0);
        /* XXX TODO encoded terminal modes (p2, plen2) */
        p1[plen1] = 0;
        p2[plen2] = 0;
        if (!channel_openterminal(p1, a, b, x, y)) {
            log_w1("unable to open terminal");
            log_d3("packet=SSH_MSG_CHANNEL_REQUEST, pty-req ", p1, ", rejected");
            goto reject;
        }
        log_d3("packet=SSH_MSG_CHANNEL_REQUEST, pty-req ", p1, ", accepted");
        goto accept;
    }

    if (str_equaln(cmd, cmdlen, "window-change")) {
/*
      byte      SSH_MSG_CHANNEL_REQUEST
      uint32    recipient channel
      string    "window-change"
      boolean   FALSE
      uint32    terminal width, columns
      uint32    terminal height, rows
      uint32    terminal width, pixels
      uint32    terminal height, pixels
*/
        pos = packetparser_uint32(b1->buf, b1->len, pos, &a);
        pos = packetparser_uint32(b1->buf, b1->len, pos, &b);
        pos = packetparser_uint32(b1->buf, b1->len, pos, &x);
        pos = packetparser_uint32(b1->buf, b1->len, pos, &y);
        pos = packetparser_end(b1->buf, b1->len, pos);

        channel_ptyresize(a, b, x, y);
        log_d1("packet=SSH_MSG_CHANNEL_REQUEST, window-change, accepted");
        goto accept;
    }

    log_d3("packet=SSH_MSG_CHANNEL_REQUEST, ", cmd, ", rejected or ignored");

reject:
    /* reject channel request */
    if (!wantreply) goto done;
    buf_purge(b2);
    buf_putnum8(b2, SSH_MSG_CHANNEL_FAILURE);      /* byte      SSH_MSG_CHANNEL_SUCCESS */
    buf_putnum32(b2, id);                          /* uint32    recipient channel */
    packet_put(b2);
    buf_purge(b1); buf_purge(b2);
    return 1;

accept:
    if (!wantreply) goto done;
    buf_purge(b2);
    buf_putnum8(b2, SSH_MSG_CHANNEL_SUCCESS);      /* byte      SSH_MSG_CHANNEL_SUCCESS */
    buf_putnum32(b2, id);                          /* uint32    recipient channel */
    packet_put(b2);
    buf_purge(b1); buf_purge(b2);
    return 1;

done:
    buf_purge(b1); buf_purge(b2);
    return 1;
}
