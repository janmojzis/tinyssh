CC?=cc
CFLAGS+=-W -Wall -Os -fPIC -fwrapv -Icryptoint
LDFLAGS?=
CPPFLAGS?=

DESTDIR?=
PREFIX?=/usr/local

INSTALL?=install

LINKS=tinysshd-makekey tinysshd-printkey tinysshnoneauthd

BINARIES=tinysshd _tinysshd-printkex _tinysshd-speed _tinysshd-test-hello1 \
 _tinysshd-test-hello2 _tinysshd-test-kex1 _tinysshd-test-kex2 \
 _tinysshd-unauthenticated

TESTCRYPTOBINARIES=test-crypto

OBJLIB=blocking.o buf.o byte.o channel.o channel_drop.o channel_fork.o \
 channel_forkpty.o channel_subsystem.o cleanup.o coe.o connectioninfo.o \
 crypto_dh_x25519.o crypto_hash_sha256.o crypto_hash_sha512_lib25519.o \
 crypto_hash_sha512_tinyssh.o crypto_kem_sntrup761_libntruprime.o \
 crypto_kem_sntrup761_tinyssh.o crypto_kem_sntrup761x25519.o \
 crypto_onetimeauth_poly1305_lib1305.o crypto_onetimeauth_poly1305_tinyssh.o \
 crypto_scalarmult_curve25519.o crypto_sign_ed25519_lib25519.o \
 crypto_sign_ed25519_tinyssh.o crypto_sort_uint32.o crypto_stream_chacha20.o \
 crypto_verify_16.o crypto_verify_32.o die.o dropuidgid.o e.o env.o fe25519.o \
 fe.o ge25519.o getln.o global.o int16_optblocker.o int32_optblocker.o \
 int64_optblocker.o int8_optblocker.o iptostr.o load.o log.o loginshell.o \
 logsys.o main_tinysshd.o main_tinysshd_makekey.o main_tinysshd_printkey.o \
 newenv.o numtostr.o open.o packet_auth.o packet.o packet_channel_open.o \
 packet_channel_recv.o packet_channel_request.o packet_channel_send.o \
 packet_get.o packet_hello.o packet_kex.o packet_kexdh.o packetparser.o \
 packet_put.o packet_recv.o packet_send.o packet_unimplemented.o porttostr.o \
 randommod.o readall.o savesync.o sc25519.o ssh.o sshcrypto.o sshcrypto_cipher.o \
 sshcrypto_cipher_chachapoly.o sshcrypto_kex.o sshcrypto_kex_curve25519.o \
 sshcrypto_kex_sntrup761x25519.o sshcrypto_key.o sshcrypto_key_ed25519.o str.o \
 stringparser.o subprocess_auth.o subprocess_sign.o trymlock.o \
 uint16_optblocker.o uint32_optblocker.o uint64_optblocker.o uint8_optblocker.o \
 writeall.o

OBJALL=blocking.o buf.o byte.o channel.o channel_drop.o channel_fork.o \
 channel_forkpty.o channel_subsystem.o cleanup.o coe.o connectioninfo.o \
 crypto_dh_x25519.o crypto_hash_sha256.o crypto_hash_sha512_lib25519.o \
 crypto_hash_sha512_tinyssh.o crypto_kem_sntrup761_libntruprime.o \
 crypto_kem_sntrup761_tinyssh.o crypto_kem_sntrup761x25519.o \
 crypto_onetimeauth_poly1305_lib1305.o crypto_onetimeauth_poly1305_tinyssh.o \
 crypto_scalarmult_curve25519.o crypto_sign_ed25519_lib25519.o \
 crypto_sign_ed25519_tinyssh.o crypto_sort_uint32.o crypto_stream_chacha20.o \
 crypto_verify_16.o crypto_verify_32.o die.o dropuidgid.o e.o env.o fe25519.o \
 fe.o ge25519.o getln.o global.o int16_optblocker.o int32_optblocker.o \
 int64_optblocker.o int8_optblocker.o iptostr.o load.o log.o loginshell.o \
 logsys.o main_tinysshd.o main_tinysshd_makekey.o main_tinysshd_printkey.o \
 newenv.o numtostr.o open.o packet_auth.o packet.o packet_channel_open.o \
 packet_channel_recv.o packet_channel_request.o packet_channel_send.o \
 packet_get.o packet_hello.o packet_kex.o packet_kexdh.o packetparser.o \
 packet_put.o packet_recv.o packet_send.o packet_unimplemented.o porttostr.o \
 randombytes.o randommod.o readall.o savesync.o sc25519.o ssh.o sshcrypto.o \
 sshcrypto_cipher.o sshcrypto_cipher_chachapoly.o sshcrypto_kex.o \
 sshcrypto_kex_curve25519.o sshcrypto_kex_sntrup761x25519.o sshcrypto_key.o \
 sshcrypto_key_ed25519.o str.o stringparser.o subprocess_auth.o \
 subprocess_sign.o test-crypto.o tinysshd.o _tinysshd-printkex.o \
 _tinysshd-speed.o _tinysshd-test-hello1.o _tinysshd-test-hello2.o \
 _tinysshd-test-kex1.o _tinysshd-test-kex2.o _tinysshd-unauthenticated.o \
 trymlock.o uint16_optblocker.o uint32_optblocker.o uint64_optblocker.o \
 uint8_optblocker.o writeall.o

AUTOHEADERS=haslib1305.h haslib25519.h haslibntruprime.h haslibrandombytes.h \
 haslibutilh.h haslimits.h haslogintty.h hasmlock.h hasopenpty.h hasutilh.h \
 hasutmpaddrv6.h hasutmp.h hasutmphost.h hasutmploginlogout.h hasutmplogwtmp.h \
 hasutmpname.h hasutmppid.h hasutmptime.h hasutmptv.h hasutmptype.h \
 hasutmpuser.h hasutmpxaddrv6.h hasutmpx.h hasutmpxsyslen.h hasutmpxupdwtmpx.h \
 hasvalgrind.h

TESTOUT=test-crypto-dh.out test-crypto-hash.out test-crypto-kem.out \
 test-crypto-onetimeauth.out test-crypto-sign.out test-crypto-sort.out \
 test-crypto-verify.out test-tinysshd-makekey.out test-tinysshd-printkey.out \
 test-tinysshd.out test-tinysshnoneauthd.out

all: $(AUTOHEADERS) $(BINARIES) $(LINKS)

blocking.o: blocking.c blocking.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c blocking.c

buf.o: buf.c byte.h str.h purge.h cleanup.h randombytes.h \
 haslibrandombytes.h cryptoint/crypto_uint32.h bug.h global.h e.h log.h \
 buf.h cryptoint/crypto_uint8.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c buf.c

byte.o: byte.c cryptoint/crypto_int16.h byte.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c byte.c

channel.o: channel.c byte.h bug.h global.h e.h log.h newenv.h purge.h \
 cleanup.h connectioninfo.h iptostr.h porttostr.h buf.h \
 cryptoint/crypto_uint8.h cryptoint/crypto_uint32.h str.h logsys.h \
 loginshell.h trymlock.h limit.h haslimits.h channel.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c channel.c

channel_drop.o: channel_drop.c dropuidgid.h newenv.h channel.h \
 cryptoint/crypto_uint32.h iptostr.h porttostr.h limit.h haslimits.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c channel_drop.c

channel_fork.o: channel_fork.c blocking.h open.h channel.h \
 cryptoint/crypto_uint32.h iptostr.h porttostr.h limit.h haslimits.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c channel_fork.c

channel_forkpty.o: channel_forkpty.c hasopenpty.h haslogintty.h e.h coe.h \
 blocking.h global.h channel.h cryptoint/crypto_uint32.h iptostr.h \
 porttostr.h limit.h haslimits.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c channel_forkpty.c

channel_subsystem.o: channel_subsystem.c str.h log.h e.h channel.h \
 cryptoint/crypto_uint32.h iptostr.h porttostr.h limit.h haslimits.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c channel_subsystem.c

cleanup.o: cleanup.c cleanup.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c cleanup.c

coe.o: coe.c coe.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c coe.c

connectioninfo.o: connectioninfo.c e.h byte.h iptostr.h porttostr.h env.h \
 str.h connectioninfo.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c connectioninfo.c

crypto_dh_x25519.o: crypto_dh_x25519.c crypto_dh_x25519.h haslib25519.h \
 randombytes.h haslibrandombytes.h crypto_scalarmult_curve25519.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c crypto_dh_x25519.c

crypto_hash_sha256.o: crypto_hash_sha256.c cryptoint/crypto_uint32.h \
 cryptoint/crypto_uint64.h crypto_hash_sha256.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c crypto_hash_sha256.c

crypto_hash_sha512_lib25519.o: crypto_hash_sha512_lib25519.c \
 haslib25519.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c crypto_hash_sha512_lib25519.c

crypto_hash_sha512_tinyssh.o: crypto_hash_sha512_tinyssh.c \
 crypto_hash_sha512.h haslib25519.h cryptoint/crypto_uint64.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c crypto_hash_sha512_tinyssh.c

crypto_kem_sntrup761_libntruprime.o: crypto_kem_sntrup761_libntruprime.c \
 haslibntruprime.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c crypto_kem_sntrup761_libntruprime.c

crypto_kem_sntrup761_tinyssh.o: crypto_kem_sntrup761_tinyssh.c \
 crypto_kem_sntrup761.h haslibntruprime.h cryptoint/crypto_uint64.h \
 cryptoint/crypto_uint32.h cryptoint/crypto_uint16.h \
 cryptoint/crypto_int32.h cryptoint/crypto_int16.h \
 cryptoint/crypto_int8.h randombytes.h haslibrandombytes.h \
 crypto_verify_32.h crypto_sort_uint32.h crypto_hash_sha512.h \
 haslib25519.h crypto_declassify.h hasvalgrind.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c crypto_kem_sntrup761_tinyssh.c

crypto_kem_sntrup761x25519.o: crypto_kem_sntrup761x25519.c cleanup.h \
 crypto_hash_sha512.h haslib25519.h crypto_dh_x25519.h \
 crypto_kem_sntrup761.h haslibntruprime.h crypto_kem_sntrup761x25519.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c crypto_kem_sntrup761x25519.c

crypto_onetimeauth_poly1305_lib1305.o: \
 crypto_onetimeauth_poly1305_lib1305.c haslib1305.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c crypto_onetimeauth_poly1305_lib1305.c

crypto_onetimeauth_poly1305_tinyssh.o: \
 crypto_onetimeauth_poly1305_tinyssh.c crypto_onetimeauth_poly1305.h \
 haslib1305.h cryptoint/crypto_int16.h cryptoint/crypto_uint32.h \
 cryptoint/crypto_uint64.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c crypto_onetimeauth_poly1305_tinyssh.c

crypto_scalarmult_curve25519.o: crypto_scalarmult_curve25519.c cleanup.h \
 fe25519.h fe.h cryptoint/crypto_uint32.h cryptoint/crypto_uint64.h \
 cryptoint/crypto_uint8.h crypto_scalarmult_curve25519.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c crypto_scalarmult_curve25519.c

crypto_sign_ed25519_lib25519.o: crypto_sign_ed25519_lib25519.c \
 haslib25519.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c crypto_sign_ed25519_lib25519.c

crypto_sign_ed25519_tinyssh.o: crypto_sign_ed25519_tinyssh.c \
 crypto_sign_ed25519.h haslib25519.h randombytes.h haslibrandombytes.h \
 cleanup.h crypto_hash_sha512.h crypto_verify_32.h ge25519.h fe.h \
 cryptoint/crypto_uint32.h cryptoint/crypto_uint64.h sc25519.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c crypto_sign_ed25519_tinyssh.c

crypto_sort_uint32.o: crypto_sort_uint32.c cryptoint/crypto_uint32.h \
 crypto_sort_uint32.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c crypto_sort_uint32.c

crypto_stream_chacha20.o: crypto_stream_chacha20.c \
 crypto_stream_chacha20.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c crypto_stream_chacha20.c

crypto_verify_16.o: crypto_verify_16.c cryptoint/crypto_int16.h \
 crypto_verify_16.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c crypto_verify_16.c

crypto_verify_32.o: crypto_verify_32.c cryptoint/crypto_int16.h \
 crypto_verify_32.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c crypto_verify_32.c

die.o: die.c global.h log.h die.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c die.c

dropuidgid.o: dropuidgid.c e.h dropuidgid.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c dropuidgid.c

e.o: e.c e.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c e.c

env.o: env.c str.h env.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c env.c

fe25519.o: fe25519.c crypto_verify_32.h cleanup.h fe.h \
 cryptoint/crypto_uint32.h cryptoint/crypto_uint64.h fe25519.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c fe25519.c

fe.o: fe.c fe.h cryptoint/crypto_uint32.h cryptoint/crypto_uint64.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c fe.c

ge25519.o: ge25519.c fe25519.h fe.h cryptoint/crypto_uint32.h \
 cryptoint/crypto_uint64.h cleanup.h ge25519.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c ge25519.c

getln.o: getln.c e.h getln.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c getln.c

global.o: global.c newenv.h channel.h cryptoint/crypto_uint32.h iptostr.h \
 porttostr.h limit.h haslimits.h packet.h buf.h cryptoint/crypto_uint8.h \
 sshcrypto.h crypto.h cryptoint/crypto_int16.h cryptoint/crypto_int32.h \
 cryptoint/crypto_int64.h cryptoint/crypto_int8.h \
 cryptoint/crypto_uint16.h cryptoint/crypto_uint64.h crypto_verify_16.h \
 crypto_verify_32.h haslibrandombytes.h randombytes.h \
 crypto_hash_sha256.h crypto_hash_sha512.h haslib25519.h \
 crypto_kem_sntrup761.h haslibntruprime.h crypto_kem_sntrup761x25519.h \
 crypto_onetimeauth_poly1305.h haslib1305.h \
 crypto_scalarmult_curve25519.h crypto_dh_x25519.h crypto_sign_ed25519.h \
 crypto_sort_uint32.h crypto_stream_chacha20.h purge.h cleanup.h \
 trymlock.h global.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c global.c

int16_optblocker.o: int16_optblocker.c cryptoint/crypto_int16.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c int16_optblocker.c

int32_optblocker.o: int32_optblocker.c cryptoint/crypto_int32.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c int32_optblocker.c

int64_optblocker.o: int64_optblocker.c cryptoint/crypto_int64.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c int64_optblocker.c

int8_optblocker.o: int8_optblocker.c cryptoint/crypto_int8.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c int8_optblocker.c

iptostr.o: iptostr.c byte.h iptostr.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c iptostr.c

load.o: load.c readall.h open.h e.h load.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c load.c

log.o: log.c writeall.h randommod.h purge.h cleanup.h numtostr.h e.h \
 log.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c log.c

loginshell.o: loginshell.c e.h loginshell.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c loginshell.c

logsys.o: logsys.c hasutilh.h hasutmpx.h hasutmpxupdwtmpx.h \
 hasutmpxsyslen.h hasutmpxaddrv6.h hasutmp.h hasutmpname.h hasutmppid.h \
 hasutmptime.h hasutmptv.h hasutmptype.h hasutmphost.h hasutmpuser.h \
 hasutmplogwtmp.h hasutmploginlogout.h hasutmpaddrv6.h str.h byte.h \
 logsys.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c logsys.c

main_tinysshd.o: main_tinysshd.c blocking.h ssh.h purge.h cleanup.h \
 open.h load.h e.h byte.h buf.h cryptoint/crypto_uint8.h \
 cryptoint/crypto_uint32.h packet.h sshcrypto.h crypto.h \
 cryptoint/crypto_int16.h cryptoint/crypto_int32.h \
 cryptoint/crypto_int64.h cryptoint/crypto_int8.h \
 cryptoint/crypto_uint16.h cryptoint/crypto_uint64.h crypto_verify_16.h \
 crypto_verify_32.h haslibrandombytes.h randombytes.h \
 crypto_hash_sha256.h crypto_hash_sha512.h haslib25519.h \
 crypto_kem_sntrup761.h haslibntruprime.h crypto_kem_sntrup761x25519.h \
 crypto_onetimeauth_poly1305.h haslib1305.h \
 crypto_scalarmult_curve25519.h crypto_dh_x25519.h crypto_sign_ed25519.h \
 crypto_sort_uint32.h crypto_stream_chacha20.h limit.h haslimits.h \
 channel.h iptostr.h porttostr.h log.h subprocess.h global.h \
 connectioninfo.h die.h str.h main.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c main_tinysshd.c

main_tinysshd_makekey.o: main_tinysshd_makekey.c savesync.h log.h \
 sshcrypto.h buf.h cryptoint/crypto_uint8.h cryptoint/crypto_uint32.h \
 crypto.h cryptoint/crypto_int16.h cryptoint/crypto_int32.h \
 cryptoint/crypto_int64.h cryptoint/crypto_int8.h \
 cryptoint/crypto_uint16.h cryptoint/crypto_uint64.h crypto_verify_16.h \
 crypto_verify_32.h haslibrandombytes.h randombytes.h \
 crypto_hash_sha256.h crypto_hash_sha512.h haslib25519.h \
 crypto_kem_sntrup761.h haslibntruprime.h crypto_kem_sntrup761x25519.h \
 crypto_onetimeauth_poly1305.h haslib1305.h \
 crypto_scalarmult_curve25519.h crypto_dh_x25519.h crypto_sign_ed25519.h \
 crypto_sort_uint32.h crypto_stream_chacha20.h global.h die.h main.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c main_tinysshd_makekey.c

main_tinysshd_printkey.o: main_tinysshd_printkey.c e.h load.h sshcrypto.h \
 buf.h cryptoint/crypto_uint8.h cryptoint/crypto_uint32.h crypto.h \
 cryptoint/crypto_int16.h cryptoint/crypto_int32.h \
 cryptoint/crypto_int64.h cryptoint/crypto_int8.h \
 cryptoint/crypto_uint16.h cryptoint/crypto_uint64.h crypto_verify_16.h \
 crypto_verify_32.h haslibrandombytes.h randombytes.h \
 crypto_hash_sha256.h crypto_hash_sha512.h haslib25519.h \
 crypto_kem_sntrup761.h haslibntruprime.h crypto_kem_sntrup761x25519.h \
 crypto_onetimeauth_poly1305.h haslib1305.h \
 crypto_scalarmult_curve25519.h crypto_dh_x25519.h crypto_sign_ed25519.h \
 crypto_sort_uint32.h crypto_stream_chacha20.h global.h log.h writeall.h \
 die.h main.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c main_tinysshd_printkey.c

newenv.o: newenv.c e.h byte.h str.h purge.h cleanup.h bug.h global.h \
 log.h env.h trymlock.h newenv.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c newenv.c

numtostr.o: numtostr.c numtostr.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c numtostr.c

open.o: open.c blocking.h open.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c open.c

packet_auth.o: packet_auth.c buf.h cryptoint/crypto_uint8.h \
 cryptoint/crypto_uint32.h ssh.h e.h str.h packetparser.h subprocess.h \
 sshcrypto.h crypto.h cryptoint/crypto_int16.h cryptoint/crypto_int32.h \
 cryptoint/crypto_int64.h cryptoint/crypto_int8.h \
 cryptoint/crypto_uint16.h cryptoint/crypto_uint64.h crypto_verify_16.h \
 crypto_verify_32.h haslibrandombytes.h randombytes.h \
 crypto_hash_sha256.h crypto_hash_sha512.h haslib25519.h \
 crypto_kem_sntrup761.h haslibntruprime.h crypto_kem_sntrup761x25519.h \
 crypto_onetimeauth_poly1305.h haslib1305.h \
 crypto_scalarmult_curve25519.h crypto_dh_x25519.h crypto_sign_ed25519.h \
 crypto_sort_uint32.h crypto_stream_chacha20.h bug.h global.h log.h \
 purge.h cleanup.h packet.h limit.h haslimits.h channel.h iptostr.h \
 porttostr.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c packet_auth.c

packet.o: packet.c purge.h cleanup.h trymlock.h packet.h buf.h \
 cryptoint/crypto_uint8.h cryptoint/crypto_uint32.h sshcrypto.h crypto.h \
 cryptoint/crypto_int16.h cryptoint/crypto_int32.h \
 cryptoint/crypto_int64.h cryptoint/crypto_int8.h \
 cryptoint/crypto_uint16.h cryptoint/crypto_uint64.h crypto_verify_16.h \
 crypto_verify_32.h haslibrandombytes.h randombytes.h \
 crypto_hash_sha256.h crypto_hash_sha512.h haslib25519.h \
 crypto_kem_sntrup761.h haslibntruprime.h crypto_kem_sntrup761x25519.h \
 crypto_onetimeauth_poly1305.h haslib1305.h \
 crypto_scalarmult_curve25519.h crypto_dh_x25519.h crypto_sign_ed25519.h \
 crypto_sort_uint32.h crypto_stream_chacha20.h limit.h haslimits.h \
 channel.h iptostr.h porttostr.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c packet.c

packet_channel_open.o: packet_channel_open.c buf.h \
 cryptoint/crypto_uint8.h cryptoint/crypto_uint32.h ssh.h e.h bug.h \
 global.h log.h packetparser.h str.h packet.h sshcrypto.h crypto.h \
 cryptoint/crypto_int16.h cryptoint/crypto_int32.h \
 cryptoint/crypto_int64.h cryptoint/crypto_int8.h \
 cryptoint/crypto_uint16.h cryptoint/crypto_uint64.h crypto_verify_16.h \
 crypto_verify_32.h haslibrandombytes.h randombytes.h \
 crypto_hash_sha256.h crypto_hash_sha512.h haslib25519.h \
 crypto_kem_sntrup761.h haslibntruprime.h crypto_kem_sntrup761x25519.h \
 crypto_onetimeauth_poly1305.h haslib1305.h \
 crypto_scalarmult_curve25519.h crypto_dh_x25519.h crypto_sign_ed25519.h \
 crypto_sort_uint32.h crypto_stream_chacha20.h limit.h haslimits.h \
 channel.h iptostr.h porttostr.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c packet_channel_open.c

packet_channel_recv.o: packet_channel_recv.c bug.h global.h e.h log.h \
 buf.h cryptoint/crypto_uint8.h cryptoint/crypto_uint32.h channel.h \
 iptostr.h porttostr.h limit.h haslimits.h ssh.h packetparser.h packet.h \
 sshcrypto.h crypto.h cryptoint/crypto_int16.h cryptoint/crypto_int32.h \
 cryptoint/crypto_int64.h cryptoint/crypto_int8.h \
 cryptoint/crypto_uint16.h cryptoint/crypto_uint64.h crypto_verify_16.h \
 crypto_verify_32.h haslibrandombytes.h randombytes.h \
 crypto_hash_sha256.h crypto_hash_sha512.h haslib25519.h \
 crypto_kem_sntrup761.h haslibntruprime.h crypto_kem_sntrup761x25519.h \
 crypto_onetimeauth_poly1305.h haslib1305.h \
 crypto_scalarmult_curve25519.h crypto_dh_x25519.h crypto_sign_ed25519.h \
 crypto_sort_uint32.h crypto_stream_chacha20.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c packet_channel_recv.c

packet_channel_request.o: packet_channel_request.c buf.h \
 cryptoint/crypto_uint8.h cryptoint/crypto_uint32.h ssh.h e.h bug.h \
 global.h log.h str.h packetparser.h packet.h sshcrypto.h crypto.h \
 cryptoint/crypto_int16.h cryptoint/crypto_int32.h \
 cryptoint/crypto_int64.h cryptoint/crypto_int8.h \
 cryptoint/crypto_uint16.h cryptoint/crypto_uint64.h crypto_verify_16.h \
 crypto_verify_32.h haslibrandombytes.h randombytes.h \
 crypto_hash_sha256.h crypto_hash_sha512.h haslib25519.h \
 crypto_kem_sntrup761.h haslibntruprime.h crypto_kem_sntrup761x25519.h \
 crypto_onetimeauth_poly1305.h haslib1305.h \
 crypto_scalarmult_curve25519.h crypto_dh_x25519.h crypto_sign_ed25519.h \
 crypto_sort_uint32.h crypto_stream_chacha20.h limit.h haslimits.h \
 channel.h iptostr.h porttostr.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c packet_channel_request.c

packet_channel_send.o: packet_channel_send.c bug.h global.h e.h log.h \
 buf.h cryptoint/crypto_uint8.h cryptoint/crypto_uint32.h channel.h \
 iptostr.h porttostr.h limit.h haslimits.h ssh.h packet.h sshcrypto.h \
 crypto.h cryptoint/crypto_int16.h cryptoint/crypto_int32.h \
 cryptoint/crypto_int64.h cryptoint/crypto_int8.h \
 cryptoint/crypto_uint16.h cryptoint/crypto_uint64.h crypto_verify_16.h \
 crypto_verify_32.h haslibrandombytes.h randombytes.h \
 crypto_hash_sha256.h crypto_hash_sha512.h haslib25519.h \
 crypto_kem_sntrup761.h haslibntruprime.h crypto_kem_sntrup761x25519.h \
 crypto_onetimeauth_poly1305.h haslib1305.h \
 crypto_scalarmult_curve25519.h crypto_dh_x25519.h crypto_sign_ed25519.h \
 crypto_sort_uint32.h crypto_stream_chacha20.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c packet_channel_send.c

packet_get.o: packet_get.c byte.h e.h bug.h global.h log.h \
 cryptoint/crypto_uint32.h purge.h cleanup.h ssh.h sshcrypto.h buf.h \
 cryptoint/crypto_uint8.h crypto.h cryptoint/crypto_int16.h \
 cryptoint/crypto_int32.h cryptoint/crypto_int64.h \
 cryptoint/crypto_int8.h cryptoint/crypto_uint16.h \
 cryptoint/crypto_uint64.h crypto_verify_16.h crypto_verify_32.h \
 haslibrandombytes.h randombytes.h crypto_hash_sha256.h \
 crypto_hash_sha512.h haslib25519.h crypto_kem_sntrup761.h \
 haslibntruprime.h crypto_kem_sntrup761x25519.h \
 crypto_onetimeauth_poly1305.h haslib1305.h \
 crypto_scalarmult_curve25519.h crypto_dh_x25519.h crypto_sign_ed25519.h \
 crypto_sort_uint32.h crypto_stream_chacha20.h numtostr.h packet.h \
 limit.h haslimits.h channel.h iptostr.h porttostr.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c packet_get.c

packet_hello.o: packet_hello.c buf.h cryptoint/crypto_uint8.h \
 cryptoint/crypto_uint32.h byte.h writeall.h purge.h cleanup.h log.h \
 str.h getln.h e.h packet.h sshcrypto.h crypto.h cryptoint/crypto_int16.h \
 cryptoint/crypto_int32.h cryptoint/crypto_int64.h \
 cryptoint/crypto_int8.h cryptoint/crypto_uint16.h \
 cryptoint/crypto_uint64.h crypto_verify_16.h crypto_verify_32.h \
 haslibrandombytes.h randombytes.h crypto_hash_sha256.h \
 crypto_hash_sha512.h haslib25519.h crypto_kem_sntrup761.h \
 haslibntruprime.h crypto_kem_sntrup761x25519.h \
 crypto_onetimeauth_poly1305.h haslib1305.h \
 crypto_scalarmult_curve25519.h crypto_dh_x25519.h crypto_sign_ed25519.h \
 crypto_sort_uint32.h crypto_stream_chacha20.h limit.h haslimits.h \
 channel.h iptostr.h porttostr.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c packet_hello.c

packet_kex.o: packet_kex.c buf.h cryptoint/crypto_uint8.h \
 cryptoint/crypto_uint32.h ssh.h sshcrypto.h crypto.h \
 cryptoint/crypto_int16.h cryptoint/crypto_int32.h \
 cryptoint/crypto_int64.h cryptoint/crypto_int8.h \
 cryptoint/crypto_uint16.h cryptoint/crypto_uint64.h crypto_verify_16.h \
 crypto_verify_32.h haslibrandombytes.h randombytes.h \
 crypto_hash_sha256.h crypto_hash_sha512.h haslib25519.h \
 crypto_kem_sntrup761.h haslibntruprime.h crypto_kem_sntrup761x25519.h \
 crypto_onetimeauth_poly1305.h haslib1305.h \
 crypto_scalarmult_curve25519.h crypto_dh_x25519.h crypto_sign_ed25519.h \
 crypto_sort_uint32.h crypto_stream_chacha20.h packetparser.h bug.h \
 global.h e.h log.h packet.h limit.h haslimits.h channel.h iptostr.h \
 porttostr.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c packet_kex.c

packet_kexdh.o: packet_kexdh.c buf.h cryptoint/crypto_uint8.h \
 cryptoint/crypto_uint32.h ssh.h e.h byte.h sshcrypto.h crypto.h \
 cryptoint/crypto_int16.h cryptoint/crypto_int32.h \
 cryptoint/crypto_int64.h cryptoint/crypto_int8.h \
 cryptoint/crypto_uint16.h cryptoint/crypto_uint64.h crypto_verify_16.h \
 crypto_verify_32.h haslibrandombytes.h randombytes.h \
 crypto_hash_sha256.h crypto_hash_sha512.h haslib25519.h \
 crypto_kem_sntrup761.h haslibntruprime.h crypto_kem_sntrup761x25519.h \
 crypto_onetimeauth_poly1305.h haslib1305.h \
 crypto_scalarmult_curve25519.h crypto_dh_x25519.h crypto_sign_ed25519.h \
 crypto_sort_uint32.h crypto_stream_chacha20.h packetparser.h bug.h \
 global.h log.h purge.h cleanup.h subprocess.h packet.h limit.h \
 haslimits.h channel.h iptostr.h porttostr.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c packet_kexdh.c

packetparser.o: packetparser.c e.h cryptoint/crypto_uint32.h bug.h \
 global.h log.h byte.h packetparser.h cryptoint/crypto_uint8.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c packetparser.c

packet_put.o: packet_put.c cryptoint/crypto_uint32.h buf.h \
 cryptoint/crypto_uint8.h bug.h global.h e.h log.h sshcrypto.h crypto.h \
 cryptoint/crypto_int16.h cryptoint/crypto_int32.h \
 cryptoint/crypto_int64.h cryptoint/crypto_int8.h \
 cryptoint/crypto_uint16.h cryptoint/crypto_uint64.h crypto_verify_16.h \
 crypto_verify_32.h haslibrandombytes.h randombytes.h \
 crypto_hash_sha256.h crypto_hash_sha512.h haslib25519.h \
 crypto_kem_sntrup761.h haslibntruprime.h crypto_kem_sntrup761x25519.h \
 crypto_onetimeauth_poly1305.h haslib1305.h \
 crypto_scalarmult_curve25519.h crypto_dh_x25519.h crypto_sign_ed25519.h \
 crypto_sort_uint32.h crypto_stream_chacha20.h ssh.h packet.h limit.h \
 haslimits.h channel.h iptostr.h porttostr.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c packet_put.c

packet_recv.o: packet_recv.c e.h buf.h cryptoint/crypto_uint8.h \
 cryptoint/crypto_uint32.h purge.h cleanup.h packet.h sshcrypto.h \
 crypto.h cryptoint/crypto_int16.h cryptoint/crypto_int32.h \
 cryptoint/crypto_int64.h cryptoint/crypto_int8.h \
 cryptoint/crypto_uint16.h cryptoint/crypto_uint64.h crypto_verify_16.h \
 crypto_verify_32.h haslibrandombytes.h randombytes.h \
 crypto_hash_sha256.h crypto_hash_sha512.h haslib25519.h \
 crypto_kem_sntrup761.h haslibntruprime.h crypto_kem_sntrup761x25519.h \
 crypto_onetimeauth_poly1305.h haslib1305.h \
 crypto_scalarmult_curve25519.h crypto_dh_x25519.h crypto_sign_ed25519.h \
 crypto_sort_uint32.h crypto_stream_chacha20.h limit.h haslimits.h \
 channel.h iptostr.h porttostr.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c packet_recv.c

packet_send.o: packet_send.c writeall.h e.h byte.h purge.h cleanup.h \
 packet.h buf.h cryptoint/crypto_uint8.h cryptoint/crypto_uint32.h \
 sshcrypto.h crypto.h cryptoint/crypto_int16.h cryptoint/crypto_int32.h \
 cryptoint/crypto_int64.h cryptoint/crypto_int8.h \
 cryptoint/crypto_uint16.h cryptoint/crypto_uint64.h crypto_verify_16.h \
 crypto_verify_32.h haslibrandombytes.h randombytes.h \
 crypto_hash_sha256.h crypto_hash_sha512.h haslib25519.h \
 crypto_kem_sntrup761.h haslibntruprime.h crypto_kem_sntrup761x25519.h \
 crypto_onetimeauth_poly1305.h haslib1305.h \
 crypto_scalarmult_curve25519.h crypto_dh_x25519.h crypto_sign_ed25519.h \
 crypto_sort_uint32.h crypto_stream_chacha20.h limit.h haslimits.h \
 channel.h iptostr.h porttostr.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c packet_send.c

packet_unimplemented.o: packet_unimplemented.c buf.h \
 cryptoint/crypto_uint8.h cryptoint/crypto_uint32.h ssh.h log.h \
 numtostr.h packet.h sshcrypto.h crypto.h cryptoint/crypto_int16.h \
 cryptoint/crypto_int32.h cryptoint/crypto_int64.h \
 cryptoint/crypto_int8.h cryptoint/crypto_uint16.h \
 cryptoint/crypto_uint64.h crypto_verify_16.h crypto_verify_32.h \
 haslibrandombytes.h randombytes.h crypto_hash_sha256.h \
 crypto_hash_sha512.h haslib25519.h crypto_kem_sntrup761.h \
 haslibntruprime.h crypto_kem_sntrup761x25519.h \
 crypto_onetimeauth_poly1305.h haslib1305.h \
 crypto_scalarmult_curve25519.h crypto_dh_x25519.h crypto_sign_ed25519.h \
 crypto_sort_uint32.h crypto_stream_chacha20.h limit.h haslimits.h \
 channel.h iptostr.h porttostr.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c packet_unimplemented.c

porttostr.o: porttostr.c cryptoint/crypto_uint16.h porttostr.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c porttostr.c

randombytes.o: randombytes.c randombytes.h haslibrandombytes.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c randombytes.c

randommod.o: randommod.c randombytes.h haslibrandombytes.h randommod.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c randommod.c

readall.o: readall.c e.h readall.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c readall.c

savesync.o: savesync.c open.h savesync.h writeall.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c savesync.c

sc25519.o: sc25519.c cryptoint/crypto_int64.h cryptoint/crypto_uint32.h \
 cryptoint/crypto_uint64.h cleanup.h sc25519.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c sc25519.c

ssh.o: ssh.c ssh.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c ssh.c

sshcrypto.o: sshcrypto.c purge.h cleanup.h bug.h global.h e.h log.h str.h \
 sshcrypto.h buf.h cryptoint/crypto_uint8.h cryptoint/crypto_uint32.h \
 crypto.h cryptoint/crypto_int16.h cryptoint/crypto_int32.h \
 cryptoint/crypto_int64.h cryptoint/crypto_int8.h \
 cryptoint/crypto_uint16.h cryptoint/crypto_uint64.h crypto_verify_16.h \
 crypto_verify_32.h haslibrandombytes.h randombytes.h \
 crypto_hash_sha256.h crypto_hash_sha512.h haslib25519.h \
 crypto_kem_sntrup761.h haslibntruprime.h crypto_kem_sntrup761x25519.h \
 crypto_onetimeauth_poly1305.h haslib1305.h \
 crypto_scalarmult_curve25519.h crypto_dh_x25519.h crypto_sign_ed25519.h \
 crypto_sort_uint32.h crypto_stream_chacha20.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c sshcrypto.c

sshcrypto_cipher.o: sshcrypto_cipher.c crypto.h cryptoint/crypto_int16.h \
 cryptoint/crypto_int32.h cryptoint/crypto_int64.h \
 cryptoint/crypto_int8.h cryptoint/crypto_uint16.h \
 cryptoint/crypto_uint32.h cryptoint/crypto_uint64.h \
 cryptoint/crypto_uint8.h crypto_verify_16.h crypto_verify_32.h \
 haslibrandombytes.h randombytes.h crypto_hash_sha256.h \
 crypto_hash_sha512.h haslib25519.h crypto_kem_sntrup761.h \
 haslibntruprime.h crypto_kem_sntrup761x25519.h \
 crypto_onetimeauth_poly1305.h haslib1305.h \
 crypto_scalarmult_curve25519.h crypto_dh_x25519.h crypto_sign_ed25519.h \
 crypto_sort_uint32.h crypto_stream_chacha20.h stringparser.h e.h log.h \
 bug.h global.h str.h byte.h packet.h buf.h sshcrypto.h limit.h \
 haslimits.h channel.h iptostr.h porttostr.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c sshcrypto_cipher.c

sshcrypto_cipher_chachapoly.o: sshcrypto_cipher_chachapoly.c \
 cryptoint/crypto_uint32.h crypto_verify_16.h randommod.h e.h byte.h \
 purge.h cleanup.h sshcrypto.h buf.h cryptoint/crypto_uint8.h crypto.h \
 cryptoint/crypto_int16.h cryptoint/crypto_int32.h \
 cryptoint/crypto_int64.h cryptoint/crypto_int8.h \
 cryptoint/crypto_uint16.h cryptoint/crypto_uint64.h crypto_verify_32.h \
 haslibrandombytes.h randombytes.h crypto_hash_sha256.h \
 crypto_hash_sha512.h haslib25519.h crypto_kem_sntrup761.h \
 haslibntruprime.h crypto_kem_sntrup761x25519.h \
 crypto_onetimeauth_poly1305.h haslib1305.h \
 crypto_scalarmult_curve25519.h crypto_dh_x25519.h crypto_sign_ed25519.h \
 crypto_sort_uint32.h crypto_stream_chacha20.h bug.h global.h log.h \
 packet.h limit.h haslimits.h channel.h iptostr.h porttostr.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c sshcrypto_cipher_chachapoly.c

sshcrypto_kex.o: sshcrypto_kex.c crypto.h cryptoint/crypto_int16.h \
 cryptoint/crypto_int32.h cryptoint/crypto_int64.h \
 cryptoint/crypto_int8.h cryptoint/crypto_uint16.h \
 cryptoint/crypto_uint32.h cryptoint/crypto_uint64.h \
 cryptoint/crypto_uint8.h crypto_verify_16.h crypto_verify_32.h \
 haslibrandombytes.h randombytes.h crypto_hash_sha256.h \
 crypto_hash_sha512.h haslib25519.h crypto_kem_sntrup761.h \
 haslibntruprime.h crypto_kem_sntrup761x25519.h \
 crypto_onetimeauth_poly1305.h haslib1305.h \
 crypto_scalarmult_curve25519.h crypto_dh_x25519.h crypto_sign_ed25519.h \
 crypto_sort_uint32.h crypto_stream_chacha20.h stringparser.h str.h \
 byte.h e.h log.h bug.h global.h sshcrypto.h buf.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c sshcrypto_kex.c

sshcrypto_kex_curve25519.o: sshcrypto_kex_curve25519.c buf.h \
 cryptoint/crypto_uint8.h cryptoint/crypto_uint32.h crypto.h \
 cryptoint/crypto_int16.h cryptoint/crypto_int32.h \
 cryptoint/crypto_int64.h cryptoint/crypto_int8.h \
 cryptoint/crypto_uint16.h cryptoint/crypto_uint64.h crypto_verify_16.h \
 crypto_verify_32.h haslibrandombytes.h randombytes.h \
 crypto_hash_sha256.h crypto_hash_sha512.h haslib25519.h \
 crypto_kem_sntrup761.h haslibntruprime.h crypto_kem_sntrup761x25519.h \
 crypto_onetimeauth_poly1305.h haslib1305.h \
 crypto_scalarmult_curve25519.h crypto_dh_x25519.h crypto_sign_ed25519.h \
 crypto_sort_uint32.h crypto_stream_chacha20.h purge.h cleanup.h \
 sshcrypto.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c sshcrypto_kex_curve25519.c

sshcrypto_kex_sntrup761x25519.o: sshcrypto_kex_sntrup761x25519.c buf.h \
 cryptoint/crypto_uint8.h cryptoint/crypto_uint32.h crypto.h \
 cryptoint/crypto_int16.h cryptoint/crypto_int32.h \
 cryptoint/crypto_int64.h cryptoint/crypto_int8.h \
 cryptoint/crypto_uint16.h cryptoint/crypto_uint64.h crypto_verify_16.h \
 crypto_verify_32.h haslibrandombytes.h randombytes.h \
 crypto_hash_sha256.h crypto_hash_sha512.h haslib25519.h \
 crypto_kem_sntrup761.h haslibntruprime.h crypto_kem_sntrup761x25519.h \
 crypto_onetimeauth_poly1305.h haslib1305.h \
 crypto_scalarmult_curve25519.h crypto_dh_x25519.h crypto_sign_ed25519.h \
 crypto_sort_uint32.h crypto_stream_chacha20.h sshcrypto.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c sshcrypto_kex_sntrup761x25519.c

sshcrypto_key.o: sshcrypto_key.c crypto.h cryptoint/crypto_int16.h \
 cryptoint/crypto_int32.h cryptoint/crypto_int64.h \
 cryptoint/crypto_int8.h cryptoint/crypto_uint16.h \
 cryptoint/crypto_uint32.h cryptoint/crypto_uint64.h \
 cryptoint/crypto_uint8.h crypto_verify_16.h crypto_verify_32.h \
 haslibrandombytes.h randombytes.h crypto_hash_sha256.h \
 crypto_hash_sha512.h haslib25519.h crypto_kem_sntrup761.h \
 haslibntruprime.h crypto_kem_sntrup761x25519.h \
 crypto_onetimeauth_poly1305.h haslib1305.h \
 crypto_scalarmult_curve25519.h crypto_dh_x25519.h crypto_sign_ed25519.h \
 crypto_sort_uint32.h crypto_stream_chacha20.h packetparser.h \
 stringparser.h str.h byte.h e.h log.h bug.h global.h purge.h cleanup.h \
 sshcrypto.h buf.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c sshcrypto_key.c

sshcrypto_key_ed25519.o: sshcrypto_key_ed25519.c crypto.h \
 cryptoint/crypto_int16.h cryptoint/crypto_int32.h \
 cryptoint/crypto_int64.h cryptoint/crypto_int8.h \
 cryptoint/crypto_uint16.h cryptoint/crypto_uint32.h \
 cryptoint/crypto_uint64.h cryptoint/crypto_uint8.h crypto_verify_16.h \
 crypto_verify_32.h haslibrandombytes.h randombytes.h \
 crypto_hash_sha256.h crypto_hash_sha512.h haslib25519.h \
 crypto_kem_sntrup761.h haslibntruprime.h crypto_kem_sntrup761x25519.h \
 crypto_onetimeauth_poly1305.h haslib1305.h \
 crypto_scalarmult_curve25519.h crypto_dh_x25519.h crypto_sign_ed25519.h \
 crypto_sort_uint32.h crypto_stream_chacha20.h packetparser.h buf.h \
 byte.h str.h purge.h cleanup.h sshcrypto.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c sshcrypto_key_ed25519.c

str.o: str.c str.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c str.c

stringparser.o: stringparser.c byte.h e.h bug.h global.h log.h \
 stringparser.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c stringparser.c

subprocess_auth.o: subprocess_auth.c str.h open.h dropuidgid.h global.h \
 e.h log.h sshcrypto.h buf.h cryptoint/crypto_uint8.h \
 cryptoint/crypto_uint32.h crypto.h cryptoint/crypto_int16.h \
 cryptoint/crypto_int32.h cryptoint/crypto_int64.h \
 cryptoint/crypto_int8.h cryptoint/crypto_uint16.h \
 cryptoint/crypto_uint64.h crypto_verify_16.h crypto_verify_32.h \
 haslibrandombytes.h randombytes.h crypto_hash_sha256.h \
 crypto_hash_sha512.h haslib25519.h crypto_kem_sntrup761.h \
 haslibntruprime.h crypto_kem_sntrup761x25519.h \
 crypto_onetimeauth_poly1305.h haslib1305.h \
 crypto_scalarmult_curve25519.h crypto_dh_x25519.h crypto_sign_ed25519.h \
 crypto_sort_uint32.h crypto_stream_chacha20.h getln.h bug.h limit.h \
 haslimits.h subprocess.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c subprocess_auth.c

subprocess_sign.o: subprocess_sign.c load.h log.h open.h writeall.h \
 purge.h cleanup.h global.h bug.h e.h readall.h blocking.h sshcrypto.h \
 buf.h cryptoint/crypto_uint8.h cryptoint/crypto_uint32.h crypto.h \
 cryptoint/crypto_int16.h cryptoint/crypto_int32.h \
 cryptoint/crypto_int64.h cryptoint/crypto_int8.h \
 cryptoint/crypto_uint16.h cryptoint/crypto_uint64.h crypto_verify_16.h \
 crypto_verify_32.h haslibrandombytes.h randombytes.h \
 crypto_hash_sha256.h crypto_hash_sha512.h haslib25519.h \
 crypto_kem_sntrup761.h haslibntruprime.h crypto_kem_sntrup761x25519.h \
 crypto_onetimeauth_poly1305.h haslib1305.h \
 crypto_scalarmult_curve25519.h crypto_dh_x25519.h crypto_sign_ed25519.h \
 crypto_sort_uint32.h crypto_stream_chacha20.h subprocess.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c subprocess_sign.c

test-crypto.o: test-crypto.c cryptoint/crypto_uint8.h \
 cryptoint/crypto_uint32.h cryptoint/crypto_uint64.h crypto_declassify.h \
 hasvalgrind.h crypto.h cryptoint/crypto_int16.h cryptoint/crypto_int32.h \
 cryptoint/crypto_int64.h cryptoint/crypto_int8.h \
 cryptoint/crypto_uint16.h crypto_verify_16.h crypto_verify_32.h \
 haslibrandombytes.h randombytes.h crypto_hash_sha256.h \
 crypto_hash_sha512.h haslib25519.h crypto_kem_sntrup761.h \
 haslibntruprime.h crypto_kem_sntrup761x25519.h \
 crypto_onetimeauth_poly1305.h haslib1305.h \
 crypto_scalarmult_curve25519.h crypto_dh_x25519.h crypto_sign_ed25519.h \
 crypto_sort_uint32.h crypto_stream_chacha20.h _crypto-test_verify_16.inc \
 _crypto-test_verify_32.inc _crypto-test_sort_uint32.inc \
 _crypto-test_hash_sha256.inc _crypto-test_hash_sha512.inc \
 _crypto-test_sign_ed25519.inc _crypto-test_kem_sntrup761.inc \
 _crypto-test_dh_x25519.inc _crypto-test_onetimeauth_poly1305.inc
	$(CC) $(CFLAGS) $(CPPFLAGS) -c test-crypto.c

tinysshd.o: tinysshd.c str.h main.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c tinysshd.c

_tinysshd-printkex.o: _tinysshd-printkex.c log.h packet.h buf.h \
 cryptoint/crypto_uint8.h cryptoint/crypto_uint32.h sshcrypto.h crypto.h \
 cryptoint/crypto_int16.h cryptoint/crypto_int32.h \
 cryptoint/crypto_int64.h cryptoint/crypto_int8.h \
 cryptoint/crypto_uint16.h cryptoint/crypto_uint64.h crypto_verify_16.h \
 crypto_verify_32.h haslibrandombytes.h randombytes.h \
 crypto_hash_sha256.h crypto_hash_sha512.h haslib25519.h \
 crypto_kem_sntrup761.h haslibntruprime.h crypto_kem_sntrup761x25519.h \
 crypto_onetimeauth_poly1305.h haslib1305.h \
 crypto_scalarmult_curve25519.h crypto_dh_x25519.h crypto_sign_ed25519.h \
 crypto_sort_uint32.h crypto_stream_chacha20.h limit.h haslimits.h \
 channel.h iptostr.h porttostr.h ssh.h bug.h global.h e.h packetparser.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c _tinysshd-printkex.c

_tinysshd-speed.o: _tinysshd-speed.c crypto.h cryptoint/crypto_int16.h \
 cryptoint/crypto_int32.h cryptoint/crypto_int64.h \
 cryptoint/crypto_int8.h cryptoint/crypto_uint16.h \
 cryptoint/crypto_uint32.h cryptoint/crypto_uint64.h \
 cryptoint/crypto_uint8.h crypto_verify_16.h crypto_verify_32.h \
 haslibrandombytes.h randombytes.h crypto_hash_sha256.h \
 crypto_hash_sha512.h haslib25519.h crypto_kem_sntrup761.h \
 haslibntruprime.h crypto_kem_sntrup761x25519.h \
 crypto_onetimeauth_poly1305.h haslib1305.h \
 crypto_scalarmult_curve25519.h crypto_dh_x25519.h crypto_sign_ed25519.h \
 crypto_sort_uint32.h crypto_stream_chacha20.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c _tinysshd-speed.c

_tinysshd-test-hello1.o: _tinysshd-test-hello1.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c _tinysshd-test-hello1.c

_tinysshd-test-hello2.o: _tinysshd-test-hello2.c log.h packet.h buf.h \
 cryptoint/crypto_uint8.h cryptoint/crypto_uint32.h sshcrypto.h crypto.h \
 cryptoint/crypto_int16.h cryptoint/crypto_int32.h \
 cryptoint/crypto_int64.h cryptoint/crypto_int8.h \
 cryptoint/crypto_uint16.h cryptoint/crypto_uint64.h crypto_verify_16.h \
 crypto_verify_32.h haslibrandombytes.h randombytes.h \
 crypto_hash_sha256.h crypto_hash_sha512.h haslib25519.h \
 crypto_kem_sntrup761.h haslibntruprime.h crypto_kem_sntrup761x25519.h \
 crypto_onetimeauth_poly1305.h haslib1305.h \
 crypto_scalarmult_curve25519.h crypto_dh_x25519.h crypto_sign_ed25519.h \
 crypto_sort_uint32.h crypto_stream_chacha20.h limit.h haslimits.h \
 channel.h iptostr.h porttostr.h global.h str.h writeall.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c _tinysshd-test-hello2.c

_tinysshd-test-kex1.o: _tinysshd-test-kex1.c log.h packet.h buf.h \
 cryptoint/crypto_uint8.h cryptoint/crypto_uint32.h sshcrypto.h crypto.h \
 cryptoint/crypto_int16.h cryptoint/crypto_int32.h \
 cryptoint/crypto_int64.h cryptoint/crypto_int8.h \
 cryptoint/crypto_uint16.h cryptoint/crypto_uint64.h crypto_verify_16.h \
 crypto_verify_32.h haslibrandombytes.h randombytes.h \
 crypto_hash_sha256.h crypto_hash_sha512.h haslib25519.h \
 crypto_kem_sntrup761.h haslibntruprime.h crypto_kem_sntrup761x25519.h \
 crypto_onetimeauth_poly1305.h haslib1305.h \
 crypto_scalarmult_curve25519.h crypto_dh_x25519.h crypto_sign_ed25519.h \
 crypto_sort_uint32.h crypto_stream_chacha20.h limit.h haslimits.h \
 channel.h iptostr.h porttostr.h ssh.h bug.h global.h e.h packetparser.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c _tinysshd-test-kex1.c

_tinysshd-test-kex2.o: _tinysshd-test-kex2.c log.h packet.h buf.h \
 cryptoint/crypto_uint8.h cryptoint/crypto_uint32.h sshcrypto.h crypto.h \
 cryptoint/crypto_int16.h cryptoint/crypto_int32.h \
 cryptoint/crypto_int64.h cryptoint/crypto_int8.h \
 cryptoint/crypto_uint16.h cryptoint/crypto_uint64.h crypto_verify_16.h \
 crypto_verify_32.h haslibrandombytes.h randombytes.h \
 crypto_hash_sha256.h crypto_hash_sha512.h haslib25519.h \
 crypto_kem_sntrup761.h haslibntruprime.h crypto_kem_sntrup761x25519.h \
 crypto_onetimeauth_poly1305.h haslib1305.h \
 crypto_scalarmult_curve25519.h crypto_dh_x25519.h crypto_sign_ed25519.h \
 crypto_sort_uint32.h crypto_stream_chacha20.h limit.h haslimits.h \
 channel.h iptostr.h porttostr.h ssh.h bug.h global.h e.h packetparser.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c _tinysshd-test-kex2.c

_tinysshd-unauthenticated.o: _tinysshd-unauthenticated.c log.h packet.h \
 buf.h cryptoint/crypto_uint8.h cryptoint/crypto_uint32.h sshcrypto.h \
 crypto.h cryptoint/crypto_int16.h cryptoint/crypto_int32.h \
 cryptoint/crypto_int64.h cryptoint/crypto_int8.h \
 cryptoint/crypto_uint16.h cryptoint/crypto_uint64.h crypto_verify_16.h \
 crypto_verify_32.h haslibrandombytes.h randombytes.h \
 crypto_hash_sha256.h crypto_hash_sha512.h haslib25519.h \
 crypto_kem_sntrup761.h haslibntruprime.h crypto_kem_sntrup761x25519.h \
 crypto_onetimeauth_poly1305.h haslib1305.h \
 crypto_scalarmult_curve25519.h crypto_dh_x25519.h crypto_sign_ed25519.h \
 crypto_sort_uint32.h crypto_stream_chacha20.h limit.h haslimits.h \
 channel.h iptostr.h porttostr.h ssh.h bug.h global.h e.h packetparser.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c _tinysshd-unauthenticated.c

trymlock.o: trymlock.c hasmlock.h trymlock.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c trymlock.c

uint16_optblocker.o: uint16_optblocker.c cryptoint/crypto_uint16.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c uint16_optblocker.c

uint32_optblocker.o: uint32_optblocker.c cryptoint/crypto_uint32.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c uint32_optblocker.c

uint64_optblocker.o: uint64_optblocker.c cryptoint/crypto_uint64.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c uint64_optblocker.c

uint8_optblocker.o: uint8_optblocker.c cryptoint/crypto_uint8.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c uint8_optblocker.c

writeall.o: writeall.c e.h writeall.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c writeall.c

tinysshd: tinysshd.o $(OBJLIB) randombytes.o libs
	$(CC) $(CFLAGS) $(CPPFLAGS) -o tinysshd tinysshd.o \
	$(OBJLIB) $(LDFLAGS) `cat libs` randombytes.o

_tinysshd-printkex: _tinysshd-printkex.o $(OBJLIB) randombytes.o libs
	$(CC) $(CFLAGS) $(CPPFLAGS) -o _tinysshd-printkex _tinysshd-printkex.o \
	$(OBJLIB) $(LDFLAGS) `cat libs` randombytes.o

_tinysshd-speed: _tinysshd-speed.o $(OBJLIB) randombytes.o libs
	$(CC) $(CFLAGS) $(CPPFLAGS) -o _tinysshd-speed _tinysshd-speed.o \
	$(OBJLIB) $(LDFLAGS) `cat libs` randombytes.o

_tinysshd-test-hello1: _tinysshd-test-hello1.o $(OBJLIB) randombytes.o libs
	$(CC) $(CFLAGS) $(CPPFLAGS) -o _tinysshd-test-hello1 _tinysshd-test-hello1.o \
	$(OBJLIB) $(LDFLAGS) `cat libs` randombytes.o

_tinysshd-test-hello2: _tinysshd-test-hello2.o $(OBJLIB) randombytes.o libs
	$(CC) $(CFLAGS) $(CPPFLAGS) -o _tinysshd-test-hello2 _tinysshd-test-hello2.o \
	$(OBJLIB) $(LDFLAGS) `cat libs` randombytes.o

_tinysshd-test-kex1: _tinysshd-test-kex1.o $(OBJLIB) randombytes.o libs
	$(CC) $(CFLAGS) $(CPPFLAGS) -o _tinysshd-test-kex1 _tinysshd-test-kex1.o \
	$(OBJLIB) $(LDFLAGS) `cat libs` randombytes.o

_tinysshd-test-kex2: _tinysshd-test-kex2.o $(OBJLIB) randombytes.o libs
	$(CC) $(CFLAGS) $(CPPFLAGS) -o _tinysshd-test-kex2 _tinysshd-test-kex2.o \
	$(OBJLIB) $(LDFLAGS) `cat libs` randombytes.o

_tinysshd-unauthenticated: _tinysshd-unauthenticated.o $(OBJLIB) randombytes.o libs
	$(CC) $(CFLAGS) $(CPPFLAGS) -o _tinysshd-unauthenticated _tinysshd-unauthenticated.o \
	$(OBJLIB) $(LDFLAGS) `cat libs` randombytes.o


test-crypto: test-crypto.o $(OBJLIB) libs
	$(CC) $(CFLAGS) $(CPPFLAGS) -o test-crypto test-crypto.o \
	$(OBJLIB) $(LDFLAGS) `cat libs`


haslib1305.h: tryfeature.sh haslib1305.c libs
	env CC="$(CC)" CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS) `cat libs`" \
	./tryfeature.sh haslib1305.c >haslib1305.h 2>haslib1305.log
	cat haslib1305.h

haslib25519.h: tryfeature.sh haslib25519.c libs
	env CC="$(CC)" CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS) `cat libs`" \
	./tryfeature.sh haslib25519.c >haslib25519.h 2>haslib25519.log
	cat haslib25519.h

haslibntruprime.h: tryfeature.sh haslibntruprime.c libs
	env CC="$(CC)" CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS) `cat libs`" \
	./tryfeature.sh haslibntruprime.c >haslibntruprime.h 2>haslibntruprime.log
	cat haslibntruprime.h

haslibrandombytes.h: tryfeature.sh haslibrandombytes.c libs
	env CC="$(CC)" CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS) `cat libs`" \
	./tryfeature.sh haslibrandombytes.c >haslibrandombytes.h 2>haslibrandombytes.log
	cat haslibrandombytes.h

haslibutilh.h: tryfeature.sh haslibutilh.c libs
	env CC="$(CC)" CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS) `cat libs`" \
	./tryfeature.sh haslibutilh.c >haslibutilh.h 2>haslibutilh.log
	cat haslibutilh.h

haslimits.h: tryfeature.sh haslimits.c libs
	env CC="$(CC)" CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS) `cat libs`" \
	./tryfeature.sh haslimits.c >haslimits.h 2>haslimits.log
	cat haslimits.h

haslogintty.h: tryfeature.sh haslogintty.c libs
	env CC="$(CC)" CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS) `cat libs`" \
	./tryfeature.sh haslogintty.c >haslogintty.h 2>haslogintty.log
	cat haslogintty.h

hasmlock.h: tryfeature.sh hasmlock.c libs
	env CC="$(CC)" CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS) `cat libs`" \
	./tryfeature.sh hasmlock.c >hasmlock.h 2>hasmlock.log
	cat hasmlock.h

hasopenpty.h: tryfeature.sh hasopenpty.c libs
	env CC="$(CC)" CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS) `cat libs`" \
	./tryfeature.sh hasopenpty.c >hasopenpty.h 2>hasopenpty.log
	cat hasopenpty.h

hasutilh.h: tryfeature.sh hasutilh.c libs
	env CC="$(CC)" CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS) `cat libs`" \
	./tryfeature.sh hasutilh.c >hasutilh.h 2>hasutilh.log
	cat hasutilh.h

hasutmpaddrv6.h: tryfeature.sh hasutmpaddrv6.c libs
	env CC="$(CC)" CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS) `cat libs`" \
	./tryfeature.sh hasutmpaddrv6.c >hasutmpaddrv6.h 2>hasutmpaddrv6.log
	cat hasutmpaddrv6.h

hasutmp.h: tryfeature.sh hasutmp.c libs
	env CC="$(CC)" CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS) `cat libs`" \
	./tryfeature.sh hasutmp.c >hasutmp.h 2>hasutmp.log
	cat hasutmp.h

hasutmphost.h: tryfeature.sh hasutmphost.c libs
	env CC="$(CC)" CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS) `cat libs`" \
	./tryfeature.sh hasutmphost.c >hasutmphost.h 2>hasutmphost.log
	cat hasutmphost.h

hasutmploginlogout.h: tryfeature.sh hasutmploginlogout.c libs
	env CC="$(CC)" CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS) `cat libs`" \
	./tryfeature.sh hasutmploginlogout.c >hasutmploginlogout.h 2>hasutmploginlogout.log
	cat hasutmploginlogout.h

hasutmplogwtmp.h: tryfeature.sh hasutmplogwtmp.c libs
	env CC="$(CC)" CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS) `cat libs`" \
	./tryfeature.sh hasutmplogwtmp.c >hasutmplogwtmp.h 2>hasutmplogwtmp.log
	cat hasutmplogwtmp.h

hasutmpname.h: tryfeature.sh hasutmpname.c libs
	env CC="$(CC)" CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS) `cat libs`" \
	./tryfeature.sh hasutmpname.c >hasutmpname.h 2>hasutmpname.log
	cat hasutmpname.h

hasutmppid.h: tryfeature.sh hasutmppid.c libs
	env CC="$(CC)" CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS) `cat libs`" \
	./tryfeature.sh hasutmppid.c >hasutmppid.h 2>hasutmppid.log
	cat hasutmppid.h

hasutmptime.h: tryfeature.sh hasutmptime.c libs
	env CC="$(CC)" CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS) `cat libs`" \
	./tryfeature.sh hasutmptime.c >hasutmptime.h 2>hasutmptime.log
	cat hasutmptime.h

hasutmptv.h: tryfeature.sh hasutmptv.c libs
	env CC="$(CC)" CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS) `cat libs`" \
	./tryfeature.sh hasutmptv.c >hasutmptv.h 2>hasutmptv.log
	cat hasutmptv.h

hasutmptype.h: tryfeature.sh hasutmptype.c libs
	env CC="$(CC)" CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS) `cat libs`" \
	./tryfeature.sh hasutmptype.c >hasutmptype.h 2>hasutmptype.log
	cat hasutmptype.h

hasutmpuser.h: tryfeature.sh hasutmpuser.c libs
	env CC="$(CC)" CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS) `cat libs`" \
	./tryfeature.sh hasutmpuser.c >hasutmpuser.h 2>hasutmpuser.log
	cat hasutmpuser.h

hasutmpxaddrv6.h: tryfeature.sh hasutmpxaddrv6.c libs
	env CC="$(CC)" CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS) `cat libs`" \
	./tryfeature.sh hasutmpxaddrv6.c >hasutmpxaddrv6.h 2>hasutmpxaddrv6.log
	cat hasutmpxaddrv6.h

hasutmpx.h: tryfeature.sh hasutmpx.c libs
	env CC="$(CC)" CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS) `cat libs`" \
	./tryfeature.sh hasutmpx.c >hasutmpx.h 2>hasutmpx.log
	cat hasutmpx.h

hasutmpxsyslen.h: tryfeature.sh hasutmpxsyslen.c libs
	env CC="$(CC)" CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS) `cat libs`" \
	./tryfeature.sh hasutmpxsyslen.c >hasutmpxsyslen.h 2>hasutmpxsyslen.log
	cat hasutmpxsyslen.h

hasutmpxupdwtmpx.h: tryfeature.sh hasutmpxupdwtmpx.c libs
	env CC="$(CC)" CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS) `cat libs`" \
	./tryfeature.sh hasutmpxupdwtmpx.c >hasutmpxupdwtmpx.h 2>hasutmpxupdwtmpx.log
	cat hasutmpxupdwtmpx.h

hasvalgrind.h: tryfeature.sh hasvalgrind.c libs
	env CC="$(CC)" CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS) `cat libs`" \
	./tryfeature.sh hasvalgrind.c >hasvalgrind.h 2>hasvalgrind.log
	cat hasvalgrind.h

test-crypto-dh.out: $(BINARIES) $(TESTCRYPTOBINARIES) $(LINKS) runtest.sh test-crypto-dh.sh test-crypto-dh.exp
	sh runtest.sh test-crypto-dh.sh test-crypto-dh.out test-crypto-dh.exp

test-crypto-hash.out: $(BINARIES) $(TESTCRYPTOBINARIES) $(LINKS) runtest.sh test-crypto-hash.sh test-crypto-hash.exp
	sh runtest.sh test-crypto-hash.sh test-crypto-hash.out test-crypto-hash.exp

test-crypto-kem.out: $(BINARIES) $(TESTCRYPTOBINARIES) $(LINKS) runtest.sh test-crypto-kem.sh test-crypto-kem.exp
	sh runtest.sh test-crypto-kem.sh test-crypto-kem.out test-crypto-kem.exp

test-crypto-onetimeauth.out: $(BINARIES) $(TESTCRYPTOBINARIES) $(LINKS) runtest.sh test-crypto-onetimeauth.sh test-crypto-onetimeauth.exp
	sh runtest.sh test-crypto-onetimeauth.sh test-crypto-onetimeauth.out test-crypto-onetimeauth.exp

test-crypto-sign.out: $(BINARIES) $(TESTCRYPTOBINARIES) $(LINKS) runtest.sh test-crypto-sign.sh test-crypto-sign.exp
	sh runtest.sh test-crypto-sign.sh test-crypto-sign.out test-crypto-sign.exp

test-crypto-sort.out: $(BINARIES) $(TESTCRYPTOBINARIES) $(LINKS) runtest.sh test-crypto-sort.sh test-crypto-sort.exp
	sh runtest.sh test-crypto-sort.sh test-crypto-sort.out test-crypto-sort.exp

test-crypto-verify.out: $(BINARIES) $(TESTCRYPTOBINARIES) $(LINKS) runtest.sh test-crypto-verify.sh test-crypto-verify.exp
	sh runtest.sh test-crypto-verify.sh test-crypto-verify.out test-crypto-verify.exp

test-tinysshd-makekey.out: $(BINARIES) $(TESTCRYPTOBINARIES) $(LINKS) runtest.sh test-tinysshd-makekey.sh test-tinysshd-makekey.exp
	sh runtest.sh test-tinysshd-makekey.sh test-tinysshd-makekey.out test-tinysshd-makekey.exp

test-tinysshd-printkey.out: $(BINARIES) $(TESTCRYPTOBINARIES) $(LINKS) runtest.sh test-tinysshd-printkey.sh test-tinysshd-printkey.exp
	sh runtest.sh test-tinysshd-printkey.sh test-tinysshd-printkey.out test-tinysshd-printkey.exp

test-tinysshd.out: $(BINARIES) $(TESTCRYPTOBINARIES) $(LINKS) runtest.sh test-tinysshd.sh test-tinysshd.exp
	sh runtest.sh test-tinysshd.sh test-tinysshd.out test-tinysshd.exp

test-tinysshnoneauthd.out: $(BINARIES) $(TESTCRYPTOBINARIES) $(LINKS) runtest.sh test-tinysshnoneauthd.sh test-tinysshnoneauthd.exp
	sh runtest.sh test-tinysshnoneauthd.sh test-tinysshnoneauthd.out test-tinysshnoneauthd.exp

test: $(TESTOUT)

libs: trylibs.sh
	env CC="$(CC)" ./trylibs.sh -lsocket -lnsl -lutil -lrandombytes -l25519 -l1305 -lntruprime >libs 2>libs.log
	cat libs

tinysshd-makekey: tinysshd
	rm -f tinysshd-makekey
	ln -s tinysshd tinysshd-makekey

tinysshd-printkey: tinysshd
	rm -f tinysshd-printkey
	ln -s tinysshd tinysshd-printkey

tinysshnoneauthd: tinysshd
	rm -f tinysshnoneauthd
	ln -s tinysshd tinysshnoneauthd

install: $(BINARIES) $(LINKS)
	mkdir -p $(DESTDIR)$(PREFIX)/sbin
	mkdir -p $(DESTDIR)$(PREFIX)/share/man/man8
	$(INSTALL) -m 0755 tinysshd $(DESTDIR)$(PREFIX)/sbin/tinysshd
	$(INSTALL) -m 0755 tinysshd-makekey $(DESTDIR)$(PREFIX)/sbin/tinysshd-makekey
	$(INSTALL) -m 0755 tinysshd-printkey $(DESTDIR)$(PREFIX)/sbin/tinysshd-printkey
	$(INSTALL) -m 0755 tinysshnoneauthd $(DESTDIR)$(PREFIX)/sbin/tinysshnoneauthd
	$(INSTALL) -m 0644 man/tinysshd.8 $(DESTDIR)$(PREFIX)/share/man/man8/tinysshd.8
	$(INSTALL) -m 0644 man/tinysshd-makekey.8 $(DESTDIR)$(PREFIX)/share/man/man8/tinysshd-makekey.8
	$(INSTALL) -m 0644 man/tinysshd-printkey.8 $(DESTDIR)$(PREFIX)/share/man/man8/tinysshd-printkey.8
	$(INSTALL) -m 0644 man/tinysshnoneauthd.8 $(DESTDIR)$(PREFIX)/share/man/man8/tinysshnoneauthd.8

clean:
	rm -f *.log libs $(OBJLIB) $(OBJALL) $(BINARIES) $(TESTCRYPTOBINARIES) $(LINKS) $(AUTOHEADERS) $(TESTOUT)

