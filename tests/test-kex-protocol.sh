#!/bin/sh

LANG=C
export LANG
LC_ALL=C
export LC_ALL

exec 2>&1

tmpdir=$(mktemp -d "${TMPDIR-/tmp}/tinyssh-kex-protocol.XXXXXXXXXX") || exit 111
cleanup() {
    rm -rf "$tmpdir"
}
trap cleanup EXIT HUP INT TERM

./tinysshd-makekey "$tmpdir/keydir" >/dev/null 2>&1 || exit 111

echo '--- SSH KEXINIT negotiation and validation'
./_tinysshd-test-kex-protocol "$tmpdir/keydir" 2>/dev/null
echo $?
