### Introduction ###
* tinysshd is a minimalistic SSH server which implements only a subset of SSHv2 features.
* tinysshd supports only secure cryptography (minimum 128-bit security, protected against cache-timing attacks)
* tinysshd doesn't implement older crypto (such as RSA, DSA, HMAC-MD5, HMAC-SHA1, 3DES, RC4, ...)
* tinysshd doesn't implement unsafe features (such as password or hostbased authentication)
* tinysshd doesn't have features such: SSH1 protocol, compression, port forwarding, agent forwarding, X11 forwarding ...
* tinysshd uses fixed-size buffers for SSH packet processing. Authorization path resolution uses dynamically allocated memory, allocation failure rejects authorization.

### Important change since version 20260906 ###

Starting with version 20260906, TinySSH rejects all client-supplied environment variables (`env` channel requests) for security reasons. This may affect configurations using OpenSSH `SendEnv` or `SetEnv`; any required variables must be set on the server instead.

### Supported SSH algorithms ###

As of version 20260906, TinySSH supports the following SSHv2 algorithms:

| OpenSSH option | Algorithms |
| --- | --- |
| KexAlgorithms | `curve25519-sha256`, `curve25519-sha256@libssh.org`, `sntrup761x25519-sha512@openssh.com`, `sntrup761x25519-sha512` |
| Ciphers | `chacha20-poly1305@openssh.com` |
| MACs | `hmac-sha2-256` (advertised for protocol compatibility; authentication is provided by the `chacha20-poly1305@openssh.com` AEAD cipher) |
| HostKeyAlgorithms | `ssh-ed25519` |

TinySSH also supports the OpenSSH strict key-exchange extension `kex-strict-s-v00@openssh.com`.

The `curve25519-sha256` algorithms can be disabled with `-S`. The post-quantum `sntrup761x25519-sha512@openssh.com` algorithm can be disabled with `-P`.

### Project timeline ###
* <strike>experimental: 2014.01.01 - 2014.12.31 (experimentation)</strike>
* <strike>alpha(updated): 2015.01.01 - 2017.12.31 (not ready for production use, ready for testing)</strike>
* beta(updated): 2018.01.01 - ????.??.?? (ready for production use)
* stable: expected ????.??.?? - (ready for production use - including post-quantum crypto)

### Current release (20260906) ###
* has 74260 words of code
* beta release

### How-to compile and install ###
~~~
make
sudo make install
~~~

### How-to run ###
~~~
       mkdir -p /etc/tinyssh
       tinysshd-makekey /etc/tinyssh/sshkeydir
~~~
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
               ExecStart=/usr/sbin/tinysshd $TINYSSHDOPTS -- /etc/tinyssh/sshkeydir
               KillMode=process
               SuccessExitStatus=111
               StandardInput=socket
               StandardError=journal

               [Install]
               WantedBy=multi-user.target
~~~
