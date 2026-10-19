#!/bin/sh

LANG=C
export LANG
LC_ALL=C
export LC_ALL

exec 2>&1

echo '--- SSH KEXDH low-order x25519 public value rejection'
./_tinysshd-test-kex-loworder 2>/dev/null
echo $?
