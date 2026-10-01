#!/bin/sh

LANG=C
export LANG
LC_ALL=C
export LC_ALL

exec 2>&1

# Disabled until redesigned: this test depends on the permissions of the
# build directory and its parents.
echo 'SKIPPED: authorized_keys handling'
