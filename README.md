### Introduction ###

TinySSH is a minimalistic SSH server which implements only a subset of SSHv2 features.

### Features ###
* easy auditable - TinySSH has less than 100000 words of code
* no dynamic memory allocation - TinySSH has all memory statically allocated (less than 1MB)
* simple configuration - TinySSH can't be misconfigured
* reusing code - TinySSH is reusing libraries from CurveCP implementation
* reusing software - TinySSH is using systemd.socket/inetd/tcpserver/... for TCP connection
* limited amount of features - TinySSH doesn't have features such: SSH1 protocol, compression, port forwarding, agent forwarding, X11 forwarding ...
* no older cryptographic primitives - rsa, dsa, classic diffie-hellman, hmac-md5, hmac-sha1, 3des, arcfour, ...
* no copyright restrictions - TinySSH is in the public domain ([see the licence](LICENCE))

### Security features ###
* cryptographic library (minimum 128-bit security, side-channel attack resistant, state-of-the-art crypto, ...)
* public-key authentication only (no password or hostbased authentication)

### Crypto primitives ###
* State-of-the-art crypto: ssh-ed25519, curve25519-sha256@libssh.org, chacha20-poly1305@openssh.com
* Older standard: <strike>ecdsa-sha2-nistp256, ecdh-sha2-nistp256, aes256-ctr, hmac-sha2-256</strike> removed in version 20190101
* Postquantum crypto: sntrup761x25519-sha512@openssh.com, chacha20-poly1305@openssh.com

### Project timelime ###
* <strike>experimental: 2014.01.01 - 2014.12.31 (experimentation)</strike>
* <strike>alpha(updated): 2015.01.01 - 2017.12.31 (not ready for production use, ready for testing)</strike>
* beta(updated): 2018.01.01 - ????.??.?? (ready for production use)
* stable: expected ????.??.?? - (ready for production use - including post-quantum crypto)

### Current release (20210319) ###
* has 61950 words of code
* beta release

[![TravisCI status](https://travis-ci.org/janmojzis/tinyssh.svg?branch=master)](https://travis-ci.org/janmojzis/tinyssh)
