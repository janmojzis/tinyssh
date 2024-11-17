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
if ! cmp "${expfile}" "${outfile}"; then
  echo "${script} FAILED"
  if [ x"`which diff`" != x ]; then
    # print diff
    diff -u "${expfile}" "${outfile}"
  else
    # print output file
    cat "${outfile}"
  fi
  exit 1
fi

echo "${script} OK"
exit 0
