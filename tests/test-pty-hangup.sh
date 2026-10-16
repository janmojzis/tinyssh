#!/bin/sh

LANG=C
export LANG
LC_ALL=C
export LC_ALL

if test "$(id -u)" = 0; then
  echo 'test-pty-hangup: must run as a non-root user' >&2
  exit 111
fi

user=$(id -un) || exit 111

ssh=${SSH-ssh}
if ! command -v "$ssh" >/dev/null 2>&1; then
  echo 'test-pty-hangup: OpenSSH client not found' >&2
  exit 111
fi

testdir=$(pwd) || exit 111
tmpdir=$(mktemp -d "${TMPDIR-/tmp}/tinyssh-pty.XXXXXXXXXX") || exit 111
trap 'rm -rf "$tmpdir"' EXIT HUP INT TERM

keydir=$tmpdir/keydir
./tinysshd-makekey "$keydir" >/dev/null 2>&1 || exit 111

# The forced command closes the slave side of the terminal and stays alive
# while more input than the terminal accepts is pending in the server.
# The server is allowed 1 second of CPU time: it is killed if it busy-loops.
cat >"$tmpdir/server" <<EOF
#!/bin/sh
ulimit -t 1
exec '$testdir/tinysshnoneauthd' -q -e 'exec 0<&- 1>&- 2>&-; sleep 3; exit 7' '$keydir'
EOF
chmod 700 "$tmpdir/server"

yes | head -c 300000 | \
  "$ssh" -F /dev/null -tt \
    -o "ProxyCommand=$tmpdir/server" \
    -o StrictHostKeyChecking=no \
    -o UserKnownHostsFile=/dev/null \
    -o PreferredAuthentications=none \
    -o PubkeyAuthentication=no \
    -o PasswordAuthentication=no \
    -o KbdInteractiveAuthentication=no \
    -o LogLevel=QUIET \
    "$user@tinyssh-pty.invalid" >/dev/null 2>&1
status=$?

if test "$status" != 7; then
  echo "test-pty-hangup: unexpected exit status: $status" >&2
  exit 111
fi

echo 'test-pty-hangup: ok'
