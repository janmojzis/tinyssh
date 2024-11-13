#!/bin/sh

echo '--- env valgrind_multiplier=1 valgrind -q --error-exitcode=99 ./_crypto-test'
env valgrind_multiplier=1 valgrind -q --error-exitcode=99 ./_crypto-test
echo $?
echo
exit 0
