/*
20260905
Public domain.
*/

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "buf.h"
#include "byte.h"
#include "channel.h"
#include "global.h"
#include "packet.h"
#include "ssh.h"
#include "sshcrypto.h"

static unsigned char space[2048];
static struct buf b;

static void enablealgorithms(void) {
    long long i;

    for (i = 0; sshcrypto_kexs[i].name; ++i) sshcrypto_kexs[i].flagenabled = 1;
    for (i = 0; sshcrypto_keys[i].name; ++i)
        sshcrypto_keys[i].sign_flagserver = 1;
    for (i = 0; sshcrypto_ciphers[i].name; ++i)
        sshcrypto_ciphers[i].flagenabled = 1;
}

static void offer(const char *kex, const char *key, const char *cipher_c2s,
                  const char *cipher_s2c, crypto_uint8 follows, int trailing) {
    buf_purge(&b);
    buf_putnum8(&b, SSH_MSG_KEXINIT);
    buf_putzerobytes(&b, 16);
    buf_putstring(&b, kex);
    buf_putstring(&b, key);
    buf_putstring(&b, cipher_c2s);
    buf_putstring(&b, cipher_s2c);
    buf_putstring(&b, "hmac-sha2-256");
    buf_putstring(&b, "hmac-sha2-256");
    buf_putstring(&b, "none");
    buf_putstring(&b, "none");
    buf_putstring(&b, "");
    buf_putstring(&b, "");
    buf_putnum8(&b, follows);
    buf_putnum32(&b, 0);
    if (trailing) buf_putnum8(&b, 0);
}

static void validoffer(const char *kex, crypto_uint8 follows) {
    offer(kex, "ssh-ed25519", "chacha20-poly1305@openssh.com",
          "chacha20-poly1305@openssh.com", follows, 0);
}

static int repeatedrekey(void) {
    unsigned char sessionid[sshcrypto_hash_MAX];
    long long i;

    global_init();
    buf_init(&b, space, sizeof space);
    enablealgorithms();
    packet.flagauthorized = 1;
    packet.flagkeys = 1;
    packet.flagrekeying = 1;
    packet.sendpacketid = 50;
    packet.receivepacketid = 60;
    channel.id = 42;
    channel.maxpacket = 1024;
    channel.localwindow = 1234;
    for (i = 0; i < (long long) sizeof sessionid; ++i)
        sessionid[i] = packet.sessionid[i] = i + 1;

    validoffer("curve25519-sha256", 0);
    if (!packet_kex_receive_rekey(&b)) return 0;
    if (packet.kexrecv.len != b.len || packet.kex_guess != 1) return 0;
    if (!packet.flagauthorized || !packet.flagkeys || !packet.flagrekeying)
        return 0;
    if (packet.sendpacketid != 50 || packet.receivepacketid != 60) return 0;
    if (channel.id != 42 || channel.maxpacket != 1024 ||
        channel.localwindow != 1234)
        return 0;
    if (!byte_isequal(packet.sessionid, sizeof sessionid, sessionid)) return 0;

    validoffer("unknown,curve25519-sha256@libssh.org", 1);
    if (!packet_kex_receive_rekey(&b)) return 0;
    if (packet.kex_guess != 0 || !packet.kex_packet_follows) return 0;

    offer("curve25519-sha256", "ssh-ed25519", "chacha20-poly1305@openssh.com",
          "unknown", 0, 0);
    return packet_kex_receive_rekey(&b) == 0;
}

static int strictrekey(void) {
    global_init();
    buf_init(&b, space, sizeof space);
    enablealgorithms();
    packet.flagrekeying = 1;
    packet.receivepacketid = 12345;

    /* strict KEX not negotiated initially: a rekey must not enable it */
    sshcrypto_kex_flags = 0;
    validoffer("curve25519-sha256,kex-strict-c-v00@openssh.com", 0);
    if (!packet_kex_receive_rekey(&b)) return 0;
    if (packet.receivepacketid != 12345) return 0;
    if (sshcrypto_kex_flags & sshcrypto_FLAGSTRICTKEX) return 0;

    /* strict KEX negotiated initially: a rekey must not disable it */
    sshcrypto_kex_flags = sshcrypto_FLAGSTRICTKEX;
    validoffer("curve25519-sha256", 0);
    if (!packet_kex_receive_rekey(&b)) return 0;
    return packet.receivepacketid == 12345 &&
           (sshcrypto_kex_flags & sshcrypto_FLAGSTRICTKEX);
}

enum fatalmode { TRAILING, TRUNCATED, WRONG_TYPE, SAME_BUFFER };

static int fatal(enum fatalmode mode) {
    int status;
    pid_t pid = fork();

    if (pid == -1) return 0;
    if (pid == 0) {
        global_init();
        buf_init(&b, space, sizeof space);
        enablealgorithms();
        validoffer("curve25519-sha256", 0);
        if (mode == TRAILING) buf_putnum8(&b, 0);
        if (mode == TRUNCATED) --b.len;
        if (mode == WRONG_TYPE) b.buf[0] = SSH_MSG_NEWKEYS;
        if (mode == SAME_BUFFER)
            packet_kex_receive_rekey(&packet.kexrecv);
        else
            packet_kex_receive_rekey(&b);
        _exit(0);
    }
    if (waitpid(pid, &status, 0) != pid) return 0;
    return WIFEXITED(status) && WEXITSTATUS(status) == 111;
}

int main(void) {
    int ok = 1;

    if (!repeatedrekey()) ok = 0;
    if (!strictrekey()) ok = 0;
    if (!fatal(TRAILING)) ok = 0;
    if (!fatal(TRUNCATED)) ok = 0;
    if (!fatal(WRONG_TYPE)) ok = 0;
    if (!fatal(SAME_BUFFER)) ok = 0;
    return ok ? 0 : 111;
}
