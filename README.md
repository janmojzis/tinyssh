### Introduction ###
* tinysshd is a minimalistic SSH server which implements only a subset of SSHv2 features.
* tinysshd supports only secure cryptography (minimum 128-bit security, protected against cache-timing attacks)
* tinysshd doesn't implement older crypto (such as RSA, DSA, HMAC-MD5, HMAC-SHA1, 3DES, RC4, ...)
* tinysshd doesn't implement unsafe features (such as password or hostbased authentication)
* tinysshd doesn't have features such: SSH1 protocol, compression, port forwarding, agent forwarding, X11 forwarding ...
* tinysshd doesn't use dynamic memory allocation (no allocation failures, etc.)

### Crypto primitives ###
* State-of-the-art crypto: ssh-ed25519, curve25519-sha256, chacha20-poly1305@openssh.com
* Older standard: <strike>ecdsa-sha2-nistp256, ecdh-sha2-nistp256, aes256-ctr, hmac-sha2-256</strike> removed in version 20190101
* Postquantum crypto: sntrup761x25519-sha512@openssh.com, chacha20-poly1305@openssh.com

### Project timelime ###
* <strike>experimental: 2014.01.01 - 2014.12.31 (experimentation)</strike>
* <strike>alpha(updated): 2015.01.01 - 2017.12.31 (not ready for production use, ready for testing)</strike>
* beta(updated): 2018.01.01 - ????.??.?? (ready for production use)
* stable: expected ????.??.?? - (ready for production use - including post-quantum crypto)

### Current release (20220305) ###
* has 62633 words of code
* beta release

### How-to run ###
~~~
       TCPSERVER
              tcpserver -HRDl0 0.0.0.0 22 /usr/sbin/tinysshd -v /etc/tinyssh/sshkeydir &

       BUSYBOX
              busybox tcpsvd 0 22 tinysshd -v /etc/tinyssh/sshkeydir &

       INETD
           /etc/inetd.conf:
               ssh stream tcp nowait root /usr/sbin/tinysshd tinysshd -l -v /etc/tinyssh/sshkeydir

       SYSTEMD
           tinysshd.socket:
               [Unit]
               Description=TinySSH server socket
               ConditionPathExists=!/etc/tinyssh/disable_tinysshd

               [Socket]
               ListenStream=22
               Accept=yes

               [Install]
               WantedBy=sockets.target

           tinysshd@.service:
               [Unit]
               Description=Tiny SSH server
               After=network.target auditd.service

               [Service]
               ExecStartPre=-/usr/sbin/tinysshd-makekey -q /etc/tinyssh/sshkeydir
               EnvironmentFile=-/etc/default/tinysshd
               ExecStart=/usr/sbin/tinysshd ${TINYSSHDOPTS} -- /etc/tinyssh/sshkeydir
               KillMode=process
               SuccessExitStatus=111
               StandardInput=socket
               StandardError=journal

               [Install]
               WantedBy=multi-user.target
~~~

[![TravisCI status](https://travis-ci.com/janmojzis/tinyssh.svg?branch=master)](https://travis-ci.com/janmojzis/tinyssh)
