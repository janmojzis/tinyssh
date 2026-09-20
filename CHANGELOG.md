### UNRELEASED
- Installed daemon signal handlers with explicit `sigaction()` semantics.
- Rejected invalid or excess subsystem registrations at startup.
- Preserved the initial SSH session identifier length across rekeys that switch between key-exchange hash algorithms of different lengths.
- Sent SSH disconnect messages for unavailable authentication services and unexpected connection-layer messages received before authentication.
- Fixed SSH channel packet size handling and removed the `PACKET_LIMIT/2` workaround.
- Applied early channel EOF after session startup
- Applied channel close to local state
- Used _SC_OPEN_MAX when closing inherited descriptors

### 20260906 (pre-release)
- tinyssh-convert: reject inconsistent Ed25519 key material in OpenSSH private-key files.
- Fixed `authorized_keys` path validation to check the opened file and every directory in its canonical path, including symlink targets.
- Added support for the `sntrup761x25519-sha512` key-exchange algorithm name.
- Documented the supported SSH algorithms in README.md and tinysshd(8).
- Rejected all client-supplied environment variables for security reasons, extending the existing restriction for forced commands configured with `-e` to every session.
- Expanded SSH protocol tests to cover packet parsing and framing, sequence numbers, key exchange and rekey, authentication, global requests, session channels, flow control and connection shutdown.
- Consolidated all SSH, cryptographic, makekey and printkey tests under `tests/` with a common build and runner; `make test` runs the complete standard suite while `make test-ssh` runs only tests requiring an SSH client, including forced-command environment isolation.
- Added handling for SSH global requests and fixed rejected-packet sequence numbers; thanks to @dirkson for the bug report.
- Fixed handling of SSH_MSG_IGNORE and SSH_MSG_DEBUG packets, including during strict key exchange; thanks to @Sven0nevS for the bug report.
- Fixed several SSH protocol compliance issues, including minimum packet padding, none-authentication user validation, NUL bytes in text lines, data after disconnect and unknown channel types; thanks to @Zhaodl1 for the detailed report.
- Fixed validation of public-key algorithm names.
- Fixed handling of channel half-closes, child output EOF and zero-increment window adjustments.
- Fixed PTY setup, ownership and cleanup error paths; thanks to @ishtiaqniloy for the bug report.
- Fixed logging of terminal types.
- Fixed self-pipe draining to prevent busy-polling.
- Fixed close-on-exec handling when opening files and random-data sources.
- Sanitised newlines in log messages.
- Fixed base64 padding when encoding data at a non-zero buffer offset.
- Fixed the fallback shell for accounts with an empty login shell.
- Fixed cleanup of removed environment entries and removed unused or duplicate channel and packet state.
- Stored client terminal dimensions as unsigned 32-bit values.
- Fixed systemd variable substitution in the tinysshd(8) example.

### 20260601
- Fixed validation of client-controlled channel parameters.
- Fixed packet buffer bounds in plain-text and chacha20-poly1305 packet handling.
- Fixed truncated log messages in subprocess_auth.
- Forced a minimal `maxpacket` value of 32 when opening channels.
- Fixed README.md systemd variable substitution example.

### 20260401
- Fixed session handling, now rejects out-of-order or duplicate channel requests.
- Fixed connection shutdown, timeout handling and subprocess waiting edge cases.
- Fixed inherited file-descriptor leak before fork.
- Fixed file descriptor leak in authorized_keys processing.
- Fixed log buffer purging and a potential out-of-bounds read in buf_putsharedsecret_.
- Updated makefilegen.sh to use gcc -MM -isystem /usr/local/include
- Moved LICENCE.md -> LICENSE.md

### 20260301
- Fixed crypto_sign_ed25519_tinyssh_open, now rejects signatures where S >= L (Ed25519 group order).

### 20250501
- Fixed minor strict kex violation, thanks to Fabian Bäumer and Marcus Brinkmann for the bug report.
- Updated cryptoint to version 20250414
- Added support for external lib1305

### 20250201
- Relicensed under CC0-1.0 OR 0BSD OR MIT-0 OR MIT
- Final reformatting using clang-format

### 20250126 (pre-release)
- Fixed more compiler warnings
- More used cryptoint

### 20241201 (pre-release)
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
