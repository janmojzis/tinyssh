### 20241201 (pre-relase)
- Switched to [cryptoint](https://libntruprime.cr.yp.to/libntruprime-20241021/cryptoint.html) 20241003
- Switched to tests from [SUPERCOP](https://bench.cr.yp.to/supercop.html) for crypto primitives
- Fixed various compiler warnings

### 20241111 (pre-relase)
- Fixed implicit-function-declaration warning packet_put.c (tnx @lanodan)
- Modification of the build system to a simple Makefile (old build system still in directory 'old')
- Added support for external librandombytes, lib25519, libntruprime library
- Dropped support for other NaCl-compatible external libraries
- Temporary disabled unit tests.

### 20240101
- fixed channel_forkpty() race condition between close(slave) in parent process and login_tty(slave) in child process
- fixed behavior when using terminal mode and stdin redirected to /dev/null 'ssh -tt -n'
- added an 'strict-key' key exchange kex-strict-s-v00@openssh.com (Mitigates CVE-2023-48795 "Terrapin attack")

### 20230101
- LICENCE update from  public-domain to CC0, public domain works differently depending on the country, to avoid it, set explicitly CC0
- fixed sysdep tests, tnx Johannes Nixdorf

### 20220801
- tinyssh/getln.c don't close fd in case of error
- use null fields in utmpx calls for logout
- do not reverse the argument order in LIBS
- fill utmp(x)'s ut_addr_v6 field when it exists
- updated crypto/crypto_onetimeauth_poly1305.c

### 20220311
- fixed build on big-endian platforms

### 20220305 (workaround release)
- use PACKET_LIMIT/2 as incoming maximum packet size, it's workaround for packets with miscalculated packet_length

### 20220222
- add tinysshnoneauthd

### 20220101
- add tinysshnoneauthd
- fixed return error message when second channel is requested
- man/tinysshd-{make,print}key.1 -> man/tinysshd-{make,print}key.8

### 20210601
- add tools/tinyssh-convert
- fix log error messages
- honor "max packet size" requested from the client

### 20210319
- removed sntrup4591761x25519-sha512@tinyssh.org
- added sntrup761x25519-sha512@openssh.com
