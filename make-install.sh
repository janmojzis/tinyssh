#!/bin/sh -e

build="`pwd`/build"
source="`pwd`"
bin="${build}/bin"
man="${build}/man"
installcmd="${bin}/_tinysshd-install"

cat "${source}/tinyssh/TARGETS" |\
while read x
do
  [ -x "${bin}/${x}" ] || \
    ( 
      echo "=== `date` === $x not compiled, compile first!"
      exit 111; 
    ) || exit 111
done || exit 111

[ -x "${installcmd}" ] || \
    ( 
      echo "=== `date` === ${installcmd} not compiled, compile first!"
      exit 111; 
    ) || exit 111

#bin
confbin="`head -1 conf-bin`"
bindir=`echo "$1/${confbin}" | sed 's,//,/,g'`
echo "=== `date` === installing bin directory ${bindir}"
mkdir -p "${bindir}" || exit 111
cat "${source}/tinyssh/TARGETS" |\
while read x
do
  "${installcmd}" "${bin}/${x}" "${bindir}" "${x}.tmp" "${x}" 1 || exit 111
  echo "=== `date` ===   installing ${bin}/${x} -> ${bindir}/${x}"
done || exit 111
echo "=== `date` === finishing"

#man
confman="`head -1 conf-man`"
mandir=`echo "$1/${confman}" | sed 's,//,/,g'`
echo "=== `date` === installing man directory ${mandir}"
mkdir -p "${bindir}" || exit 111
ls "${man}" | sort |\
while read x
do
  n=`echo "${x}" | cut -d'.' -f2`
  mkdir -p "${mandir}/man${n}" || exit 111
  "${installcmd}" "${man}/${x}" "${mandir}/man${n}" "${x}.tmp" "${x}" || exit 111
  echo "=== `date` ===   installing ${man}/${x} -> ${mandir}/man${n}/${x}"
done || exit 111
echo "=== `date` === finishing"

exit 0
