/*
20260905
Public domain.
*/

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "buf.h"
#include "crypto_uint32.h"
#include "global.h"
#include "packet.h"
#include "ssh.h"
#include "sshcrypto.h"

static unsigned char outspace[TRANSPORT_PACKET_LIMIT];
static struct buf out;

static long long putplain(long long pos, const unsigned char *payload,
                          long long payloadlen) {
    unsigned char *x = packet.recvbuf.buf + PACKET_ZEROBYTES + pos;
    crypto_uint32 packetlen = 1 + payloadlen + 10;

    crypto_uint32_store_bigendian(x, packetlen);
    x[4] = 10;
    if (payloadlen) {
        long long i;
        for (i = 0; i < payloadlen; ++i) x[5 + i] = payload[i];
    }
    for (pos = 0; pos < 10; ++pos) x[5 + payloadlen + pos] = 0;
    return packetlen + 4;
}

static void prepare(const unsigned char *payload, long long payloadlen) {
    packet_init();
    packet.recvbuf.len = PACKET_ZEROBYTES;
    packet.recvbuf.len += putplain(0, payload, payloadlen);
    buf_purge(&out);
}

static int mustfail(void (*fn)(void)) {
    int status;
    pid_t pid = fork();

    if (pid == -1) return 0;
    if (pid == 0) {
        global_init();
        buf_init(&out, outspace, sizeof outspace);
        fn();
        _exit(0);
    }
    if (waitpid(pid, &status, 0) != pid) return 0;
    return WIFEXITED(status) && WEXITSTATUS(status) == 111;
}

static int receiveincrements(void) {
    static const unsigned char payload[] = {SSH_MSG_UNIMPLEMENTED};

    prepare(payload, sizeof payload);
    if (!packet_get(&out, 0)) return 0;
    return packet.receivepacketid == 1;
}

static int incompletekeepssequence(void) {
    static const unsigned char payload[] = {SSH_MSG_UNIMPLEMENTED};

    prepare(payload, sizeof payload);
    --packet.recvbuf.len;
    if (!packet_get(&out, 0)) return 0;
    return out.len == 0 && packet.receivepacketid == 0;
}

static int ignoredincrements(void) {
    static const unsigned char ignored[] = {SSH_MSG_IGNORE};
    static const unsigned char wanted[] = {SSH_MSG_UNIMPLEMENTED};
    long long pos;

    packet_init();
    packet.recvbuf.len = PACKET_ZEROBYTES;
    pos = putplain(0, ignored, sizeof ignored);
    packet.recvbuf.len += pos;
    packet.recvbuf.len += putplain(pos, wanted, sizeof wanted);
    buf_purge(&out);
    if (!packet_get(&out, SSH_MSG_UNIMPLEMENTED)) return 0;
    return out.len == 1 && out.buf[0] == SSH_MSG_UNIMPLEMENTED &&
           packet.receivepacketid == 2;
}

static int debugincrements(void) {
    static const unsigned char debug[] = {SSH_MSG_DEBUG};
    static const unsigned char wanted[] = {SSH_MSG_UNIMPLEMENTED};
    long long pos;

    packet_init();
    packet.recvbuf.len = PACKET_ZEROBYTES;
    pos = putplain(0, debug, sizeof debug);
    packet.recvbuf.len += pos;
    packet.recvbuf.len += putplain(pos, wanted, sizeof wanted);
    buf_purge(&out);
    if (!packet_get(&out, SSH_MSG_UNIMPLEMENTED)) return 0;
    return out.len == 1 && packet.receivepacketid == 2;
}

static int disconnectstops(void) {
    static const unsigned char payload[] = {SSH_MSG_DISCONNECT};

    prepare(payload, sizeof payload);
    if (packet_get(&out, 0)) return 0;
    return packet.receivepacketid == 1;
}

static int authorizedbypasseslimit(void) {
    static const unsigned char payload[] = {SSH_MSG_UNIMPLEMENTED};

    prepare(payload, sizeof payload);
    packet.flagauthorized = 1;
    packet.unauthenticatedmessages = PACKET_UNAUTHENTICATED_MESSAGES;
    if (!packet_get(&out, 0)) return 0;
    return packet.receivepacketid == 1 &&
           packet.unauthenticatedmessages == PACKET_UNAUTHENTICATED_MESSAGES;
}

static int strictreceivereset(void) {
    static const unsigned char payload[] = {SSH_MSG_NEWKEYS};

    prepare(payload, sizeof payload);
    packet.receivepacketid = 7;
    sshcrypto_kex_flags = sshcrypto_FLAGSTRICTKEX;
    if (!packet_get(&out, SSH_MSG_NEWKEYS)) return 0;
    sshcrypto_kex_flags = 0;
    return packet.receivepacketid == 0;
}

static int nonstrictreceivecontinues(void) {
    static const unsigned char payload[] = {SSH_MSG_NEWKEYS};

    prepare(payload, sizeof payload);
    packet.receivepacketid = 7;
    sshcrypto_kex_flags = 0;
    if (!packet_get(&out, SSH_MSG_NEWKEYS)) return 0;
    return packet.receivepacketid == 8;
}

static int sendincrements(void) {
    static const unsigned char payload[] = {SSH_MSG_UNIMPLEMENTED};

    packet_init();
    buf_purge(&out);
    buf_put(&out, payload, sizeof payload);
    packet_put(&out);
    return packet.sendpacketid == 1;
}

static int strictsendreset(void) {
    static const unsigned char payload[] = {SSH_MSG_NEWKEYS};

    packet_init();
    packet.sendpacketid = 7;
    sshcrypto_kex_flags = sshcrypto_FLAGSTRICTKEX;
    buf_purge(&out);
    buf_put(&out, payload, sizeof payload);
    packet_put(&out);
    sshcrypto_kex_flags = 0;
    return packet.sendpacketid == 0;
}

static int nonstrictsendcontinues(void) {
    static const unsigned char payload[] = {SSH_MSG_NEWKEYS};

    packet_init();
    packet.sendpacketid = 7;
    sshcrypto_kex_flags = 0;
    buf_purge(&out);
    buf_put(&out, payload, sizeof payload);
    packet_put(&out);
    return packet.sendpacketid == 8;
}

static void wrongtype(void) {
    static const unsigned char payload[] = {SSH_MSG_UNIMPLEMENTED};

    prepare(payload, sizeof payload);
    packet_get(&out, SSH_MSG_NEWKEYS);
}

static void strictignorebeforekeys(void) {
    static const unsigned char payload[] = {SSH_MSG_IGNORE};

    prepare(payload, sizeof payload);
    sshcrypto_kex_flags = sshcrypto_FLAGSTRICTKEX;
    packet_get(&out, 0);
}

static void unauthenticatedlimit(void) {
    static const unsigned char payload[] = {SSH_MSG_UNIMPLEMENTED};

    prepare(payload, sizeof payload);
    packet.unauthenticatedmessages = PACKET_UNAUTHENTICATED_MESSAGES;
    packet_get(&out, 0);
}

static void strictunauthenticatedlimit(void) {
    static const unsigned char newkeys[] = {SSH_MSG_NEWKEYS};
    static const unsigned char payload[] = {SSH_MSG_UNIMPLEMENTED};
    long long pos;

    packet_init();
    packet.recvbuf.len = PACKET_ZEROBYTES;
    pos = putplain(0, newkeys, sizeof newkeys);
    packet.recvbuf.len += pos;
    packet.recvbuf.len += putplain(pos, payload, sizeof payload);
    buf_purge(&out);
    packet.unauthenticatedmessages = PACKET_UNAUTHENTICATED_MESSAGES - 1;
    sshcrypto_kex_flags = sshcrypto_FLAGSTRICTKEX;
    if (!packet_get(&out, SSH_MSG_NEWKEYS)) _exit(0);
    packet_get(&out, 0);
}

static void receiveoverflow(void) {
    static const unsigned char payload[] = {SSH_MSG_UNIMPLEMENTED};

    prepare(payload, sizeof payload);
    packet.flagauthorized = 1;
    packet.receivepacketid = ~(crypto_uint32) 0;
    packet_get(&out, 0);
}

static void sendoverflow(void) {
    static const unsigned char payload[] = {SSH_MSG_UNIMPLEMENTED};

    packet_init();
    packet.sendpacketid = ~(crypto_uint32) 0;
    buf_purge(&out);
    buf_put(&out, payload, sizeof payload);
    packet_put(&out);
}

int main(void) {
    int ok = 1;

    global_init();
    buf_init(&out, outspace, sizeof outspace);

    if (!receiveincrements()) ok = 0;
    if (!incompletekeepssequence()) ok = 0;
    if (!ignoredincrements()) ok = 0;
    if (!debugincrements()) ok = 0;
    if (!disconnectstops()) ok = 0;
    if (!authorizedbypasseslimit()) ok = 0;
    if (!strictreceivereset()) ok = 0;
    if (!nonstrictreceivecontinues()) ok = 0;
    if (!sendincrements()) ok = 0;
    if (!strictsendreset()) ok = 0;
    if (!nonstrictsendcontinues()) ok = 0;
    if (!mustfail(wrongtype)) ok = 0;
    if (!mustfail(strictignorebeforekeys)) ok = 0;
    if (!mustfail(unauthenticatedlimit)) ok = 0;
    if (!mustfail(strictunauthenticatedlimit)) ok = 0;
    if (!mustfail(receiveoverflow)) ok = 0;
    if (!mustfail(sendoverflow)) ok = 0;

    sshcrypto_kex_flags = 0;
    global_purge();
    return ok ? 0 : 111;
}
