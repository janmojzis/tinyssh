#!/bin/sh

LANG=C
export LANG
LC_ALL=C
export LC_ALL

exec 2>&1

echo '--- portable pseudoterminal setup'
./_tinysshd-test-pty
echo $?
