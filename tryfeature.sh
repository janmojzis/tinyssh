#!/bin/sh

# version 20241107

scriptname="`basename $0`"

if [ x"${CC}" = x ]; then
  echo "usage: env CC=<compiler> ${scriptname} feature.c"
  echo '$CC not set'
  exit 1
fi

if [ x"$1" = x ]; then
  echo "usage: env CC=<compiler> ${scriptname} feature.c"
  echo 'missing feature.c argument'
  exit 1
fi

# input
tryname="$1"

# output
headermacro=`echo ${tryname} | sed 's/\.c//' | sed 'y/abcdefghijklmnopqrstuvwxyz/ABCDEFGHIJKLMNOPQRSTUVWXYZ/'`

# temporary
binname="${tryname}.tmp.bin"
logname="${tryname}.tmp.log"

cleanup() {
  ex=$?
  rm -f "${binname}" "${logname}"
  exit "${ex}"
}
trap "cleanup" EXIT TERM INT

"${CC}" ${CFLAGS} -O0 -o "${binname}" "${tryname}" ${LDFLAGS} 1>"${logname}" 2>&1
if [ $? -eq 0 ]; then
  echo "#define ${headermacro} 1"
else
  echo "#undef ${headermacro}"
  cat "${logname}" >&2
fi
exit 0
