/*
20260905
Public domain.
*/

#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include "buf.h"
#include "crypto_uint32.h"
#include "global.h"
#include "packet.h"
#include "ssh.h"
#include "sshcrypto.h"

enum result { RESULT_OK, RESULT_REJECT, RESULT_FATAL };

struct offer {
    const char *kex;
    const char *key;
    const char *cipher_c2s;
    const char *cipher_s2c;
    const char *mac_c2s;
    const char *compress_c2s;
    const char *compress_s2c;
    crypto_uint8 follows;
    crypto_uint32 prior_packets;
    int trailing;
    int truncate;
    int wrong_type;
};

static void enablealgorithms(void) {
    long long i;

    for (i = 0; sshcrypto_kexs[i].name; ++i) sshcrypto_kexs[i].flagenabled = 1;
    for (i = 0; sshcrypto_keys[i].name; ++i)
        sshcrypto_keys[i].sign_flagserver = 1;
    for (i = 0; sshcrypto_ciphers[i].name; ++i)
        sshcrypto_ciphers[i].flagenabled = 1;
}

/* '!' stands for a NUL byte inside the name-list */
static void putnamelist(struct buf *b, const char *x) {
    long long i = b->len + 4;

    buf_putstring(b, x);
    for (; i < b->len; ++i)
        if (b->buf[i] == '!') b->buf[i] = 0;
}

/* append an unencrypted packet to the receive buffer */
static void putpacket(const unsigned char *payload, long long payloadlen) {
    unsigned char *x = packet.recvbuf.buf + packet.recvbuf.len;
    crypto_uint32 packetlen = 1 + payloadlen + 10;
    long long i;

    crypto_uint32_store_bigendian(x, packetlen);
    x[4] = 10;
    for (i = 0; i < payloadlen; ++i) x[5 + i] = payload[i];
    for (i = 0; i < 10; ++i) x[5 + payloadlen + i] = 0;
    packet.recvbuf.len += packetlen + 4;
}

static void makeoffer(struct buf *b, const struct offer *o) {
    buf_purge(b);
    buf_putnum8(b, o->wrong_type ? SSH_MSG_SERVICE_REQUEST : SSH_MSG_KEXINIT);
    buf_putzerobytes(b, 16);
    putnamelist(b, o->kex);
    putnamelist(b, o->key);
    putnamelist(b, o->cipher_c2s);
    buf_putstring(b, o->cipher_s2c);
    putnamelist(b, o->mac_c2s);
    buf_putstring(b, "hmac-sha2-256");
    buf_putstring(b, o->compress_c2s);
    buf_putstring(b, o->compress_s2c);
    buf_putstring(b, "");
    buf_putstring(b, "");
    buf_putnum8(b, o->follows);
    buf_putnum32(b, 0);
    if (o->trailing) buf_putnum8(b, 0);

    packet.recvbuf.len = PACKET_ZEROBYTES;
    putpacket(b->buf, b->len - o->truncate);
    packet.receivepacketid = o->prior_packets;
}

static int run(const struct offer *o, enum result expected, int expected_guess,
               int expected_strict) {
    unsigned char space[2048];
    struct buf b;
    int status;
    pid_t pid = fork();

    if (pid == -1) return 0;
    if (pid == 0) {
        int ret;

        global_init();
        enablealgorithms();
        buf_init(&b, space, sizeof space);
        makeoffer(&b, o);
        ret = packet_kex_receive();
        if (expected == RESULT_FATAL) _exit(100);
        if (!!ret != (expected == RESULT_OK)) _exit(101);
        if (ret && packet.kex_guess != expected_guess) _exit(102);
        if (ret && !!(sshcrypto_kex_flags & sshcrypto_FLAGSTRICTKEX) !=
                       expected_strict)
            _exit(103);
        if (ret && packet.kex_packet_follows != o->follows) _exit(104);
        _exit(0);
    }
    if (waitpid(pid, &status, 0) != pid) return 0;
    if (!WIFEXITED(status)) return 0;
    if (expected == RESULT_FATAL) return WEXITSTATUS(status) == 111;
    return WEXITSTATUS(status) == 0;
}

/* packet of type 'type' received between SSH_MSG_KEXDH_INIT and SSH_MSG_NEWKEYS */
static int runkexdh(const char *keydir, const struct offer *o,
                    crypto_uint8 type, int trailing, enum result expected) {
    unsigned char space1[2048];
    unsigned char space2[2048];
    struct buf b1, b2;
    int status;
    pid_t pid = fork();

    if (pid == -1) return 0;
    if (pid == 0) {
        int fd = open("/dev/null", O_WRONLY);

        if (fd == -1 || dup2(fd, 1) == -1) _exit(100);
        global_init();
        enablealgorithms();
        buf_init(&b1, space1, sizeof space1);
        buf_init(&b2, space2, sizeof space2);
        makeoffer(&b1, o);
        if (!packet_kex_receive()) _exit(100);

        buf_purge(&b1);
        buf_putnum8(&b1, SSH_MSG_KEXDH_INIT);
        buf_putnum32(&b1, 32);
        buf_putnum8(&b1, 9);
        buf_putzerobytes(&b1, 31);
        putpacket(b1.buf, b1.len);

        buf_purge(&b1);
        buf_putnum8(&b1, type);
        if (trailing) buf_putnum8(&b1, 0);
        putpacket(b1.buf, b1.len);

        buf_purge(&b1);
        buf_putnum8(&b1, SSH_MSG_NEWKEYS);
        putpacket(b1.buf, b1.len);

        _exit(packet_kexdh(keydir, &b1, &b2) ? 0 : 101);
    }
    if (waitpid(pid, &status, 0) != pid) return 0;
    if (!WIFEXITED(status)) return 0;
    if (expected == RESULT_FATAL) return WEXITSTATUS(status) == 111;
    if (expected == RESULT_REJECT) return WEXITSTATUS(status) == 101;
    return WEXITSTATUS(status) == 0;
}

int main(int argc, char **argv) {
    static const struct offer valid = {"curve25519-sha256",
                                       "ssh-ed25519",
                                       "chacha20-poly1305@openssh.com",
                                       "chacha20-poly1305@openssh.com",
                                       "hmac-sha2-256",
                                       "none",
                                       "none",
                                       0,
                                       0,
                                       0,
                                       0,
                                       0};
    struct offer o;
    int ok = 1;
    int type;

    if (argc < 2) _exit(100);

    if (!run(&valid, RESULT_OK, 1, 0)) ok = 0;

    o = valid;
    o.kex = "unknown,curve25519-sha256";
    o.follows = 1;
    if (!run(&o, RESULT_OK, 0, 0)) ok = 0;

    o = valid;
    o.kex = "curve25519-sha256,kex-strict-c-v00@openssh.com";
    if (!run(&o, RESULT_OK, 1, 1)) ok = 0;

    o.prior_packets = 1;
    if (!run(&o, RESULT_REJECT, 0, 0)) ok = 0;

    o = valid;
    o.kex = "unknown";
    if (!run(&o, RESULT_REJECT, 0, 0)) ok = 0;

    o = valid;
    o.key = "unknown";
    if (!run(&o, RESULT_REJECT, 0, 0)) ok = 0;

    o = valid;
    o.cipher_c2s = "unknown";
    if (!run(&o, RESULT_REJECT, 0, 0)) ok = 0;

    o = valid;
    o.cipher_s2c = "unknown";
    if (!run(&o, RESULT_REJECT, 0, 0)) ok = 0;

    o = valid;
    o.compress_c2s = "zlib";
    if (!run(&o, RESULT_REJECT, 0, 0)) ok = 0;

    o = valid;
    o.compress_s2c = "zlib";
    if (!run(&o, RESULT_REJECT, 0, 0)) ok = 0;

    o = valid;
    o.kex = "unknown!,curve25519-sha256";
    if (!run(&o, RESULT_FATAL, 0, 0)) ok = 0;

    o = valid;
    o.key = "unknown!,ssh-ed25519";
    if (!run(&o, RESULT_FATAL, 0, 0)) ok = 0;

    o = valid;
    o.cipher_c2s = "unknown!,chacha20-poly1305@openssh.com";
    if (!run(&o, RESULT_FATAL, 0, 0)) ok = 0;

    o = valid;
    o.mac_c2s = "unknown!,hmac-sha2-256";
    if (!run(&o, RESULT_FATAL, 0, 0)) ok = 0;

    o = valid;
    o.trailing = 1;
    if (!run(&o, RESULT_FATAL, 0, 0)) ok = 0;

    o = valid;
    o.truncate = 1;
    if (!run(&o, RESULT_FATAL, 0, 0)) ok = 0;

    o = valid;
    o.wrong_type = 1;
    if (!run(&o, RESULT_FATAL, 0, 0)) ok = 0;

    /* only SSH_MSG_NEWKEYS (or SSH_MSG_IGNORE/SSH_MSG_DEBUG) may follow
       SSH_MSG_KEXDH_INIT, with or without strict KEX */
    for (type = 1; type < 256; ++type) {
        enum result expected = RESULT_FATAL;

        if (type == SSH_MSG_DISCONNECT) expected = RESULT_REJECT;
        if (type == SSH_MSG_IGNORE || type == SSH_MSG_DEBUG ||
            type == SSH_MSG_NEWKEYS)
            expected = RESULT_OK;
        if (!runkexdh(argv[1], &valid, type, 0, expected)) ok = 0;
    }

    o = valid;
    o.kex = "curve25519-sha256,kex-strict-c-v00@openssh.com";
    if (!runkexdh(argv[1], &o, SSH_MSG_NEWKEYS, 0, RESULT_OK)) ok = 0;
    if (!runkexdh(argv[1], &o, SSH_MSG_IGNORE, 0, RESULT_FATAL)) ok = 0;
    if (!runkexdh(argv[1], &o, SSH_MSG_KEXINIT, 0, RESULT_FATAL)) ok = 0;
    if (!runkexdh(argv[1], &o, SSH_MSG_CHANNEL_DATA, 0, RESULT_FATAL)) ok = 0;

    if (!runkexdh(argv[1], &valid, SSH_MSG_NEWKEYS, 1, RESULT_FATAL))
        ok = 0;
    if (!runkexdh(argv[1], &o, SSH_MSG_NEWKEYS, 1, RESULT_FATAL)) ok = 0;

    return ok ? 0 : 111;
}
