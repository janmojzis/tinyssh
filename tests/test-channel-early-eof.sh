#!/bin/sh

LANG=C
export LANG
LC_ALL=C
export LC_ALL

exec 2>&1

echo '--- SSH channel EOF before session startup'
./_tinysshd-test-channel-early-eof 2>/dev/null
echo $?
