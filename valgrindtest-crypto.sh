#!/bin/sh

echo '--- env valgrind_multiplier=1 valgrind -q --error-exitcode=99 ./test-crypto'
env valgrind_multiplier=1 valgrind -q --error-exitcode=99 ./test-crypto
echo $?
echo
exit 0
