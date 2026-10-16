#!/bin/sh

LANG=C
export LANG
LC_ALL=C
export LC_ALL

if test "$(id -u)" = 0; then
    echo 'test-session-no-pty: must run as a non-root user' >&2
    exit 111
fi

ssh=${SSH-ssh}
if ! command -v "$ssh" >/dev/null 2>&1; then
    echo 'test-session-no-pty: OpenSSH client not found' >&2
    exit 111
fi

if ! ps -o sid= -p $$ >/dev/null 2>&1; then
    echo 'test-session-no-pty: ps -o sid not supported' >&2
    exit 111
fi

user=$(id -un) || exit 111
testdir=$(pwd) || exit 111
tmpdir=$(mktemp -d "${TMPDIR-/tmp}/tinyssh-no-pty.XXXXXXXXXX") || exit 111
trap 'rm -rf "$tmpdir"' EXIT HUP INT TERM

keydir=$tmpdir/keydir
./tinysshd-makekey "$keydir" >/dev/null 2>&1 || exit 111

# A session started without pty-req must not stay in the session of
# tinysshd and its supervisor (and keep their controlling terminal):
# the command has to be the leader of a session of its own.
proxycommand="'$testdir/tinysshnoneauthd' -q -e 'echo \$(ps -o sid= -p \$\$) \$\$' '$keydir'"

ids=$(
    "$ssh" -F /dev/null -T \
        -o "ProxyCommand=$proxycommand" \
        -o StrictHostKeyChecking=no \
        -o UserKnownHostsFile=/dev/null \
        -o PreferredAuthentications=none \
        -o PubkeyAuthentication=no \
        -o PasswordAuthentication=no \
        -o KbdInteractiveAuthentication=no \
        -o LogLevel=ERROR \
        "$user@tinyssh-no-pty.invalid" </dev/null
) || exit 111

set -- $ids
if test "$#" -ne 2 || test "$1" != "$2"; then
    echo "test-session-no-pty: command not a session leader: $ids" >&2
    exit 111
fi

echo 'test-session-no-pty: ok'
