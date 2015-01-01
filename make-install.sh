#!/bin/sh -e

build="`pwd`/build"
source="`pwd`"
bin="${build}/bin"

cat "${source}/tinyssh/TARGETS" |\
while read x
do
  [ -x "${bin}/${x}" ] || \
    ( 
      echo "=== `date` === $x not compiled, compile first!"
      exit 111; 
    ) || exit 111
done || exit 111

#bin
confbin="`head -1 conf-bin`"
echo "=== `date` === installing bin directory $1/${confbin}"
mkdir -p "$1/${confbin}" || exit 111
cat "${source}/tinyssh/TARGETS" |\
while read x
do
  cp "${bin}/${x}" "$1/${confbin}" || exit 111
  chmod 755 "$1/${confbin}/${x}" || exit 111
  chown 0:0 "$1/${confbin}/${x}" || exit 111
  echo "=== `date` ===   installing build/bin/${x} -> $1/${confbin}/${x}"
done || exit 111
echo "=== `date` === finishing"

exit 0
