#!/bin/sh
set -e

build="`pwd`/build"
source="`pwd`"
bin="${build}/bin"
man="${build}/man"

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
bindir=`echo "$1/${confbin}" | sed 's,//,/,g'`
echo "=== `date` === installing bin directory ${bindir}"
mkdir -p "${bindir}" || exit 111
cat "${source}/tinyssh/LINKS" |\
while read x y
do
  rm -f "${x}.tmp"
  if [ "_${y}" = "_" ]; then
    cp "${bin}/${x}" "${bindir}/${x}.tmp"
    (
      cd "${bindir}"
      chmod 755 "${x}.tmp"
      mv -f "${x}.tmp" "${x}"
    )
  else
    (
      cd "${bindir}"
      ln -s "${y}" "${x}.tmp"
      mv -f "${x}.tmp" "${x}"
    )
  fi
  echo "=== `date` ===   installing ${bin}/${x} -> ${bindir}/${x}"
done || exit 111
echo "=== `date` === finishing"

#man
confman="`head -1 conf-man`"
mandir=`echo "$1/${confman}" | sed 's,//,/,g'`
echo "=== `date` === installing man directory ${mandir}"
mkdir -p "${mandir}" || exit 111
ls "${man}" | sort |\
while read x
do
  n=`echo "${x}" | cut -d'.' -f2`
  mkdir -p "${mandir}/man${n}" || exit 111
  cp "${man}/${x}" "${mandir}/man${n}/${x}.tmp"
  (
    cd "${mandir}/man${n}"
    chmod 644 "${x}.tmp"
    mv -f "${x}.tmp" "${x}"
  )
  echo "=== `date` ===   installing ${man}/${x} -> ${mandir}/man${n}/${x}"
done || exit 111
echo "=== `date` === finishing"

exit 0
