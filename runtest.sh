#!/bin/sh

LANG=C
export LANG

[ x"$1" = x ] && exit 100
script="$1"
[ x"$2" = x ] && exit 100
outfile="$2"
[ x"$3" = x ] && exit 100
expfile="$3"

# run test
sh "${script}" > "${outfile}"

# compare
if cmp "${expfile}" "${outfile}"; then
  exit 0
fi

# try to print diff
if [ x"`which diff`" != x ]; then
  diff -u "${expfile}" "${outfile}"
  exit 1
fi

# print output file
cat "${outfile}"
exit 1
