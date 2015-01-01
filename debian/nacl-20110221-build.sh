#!/bin/sh

nacldir="`pwd`/debian/nacl-20110221"
naclbuild="${nacldir}/build/"

#build nacl
( cd "${nacldir}"; sh do )

#rename binaries curvecpserver -> tinysshd-curvecpserver ...
(
  cat "${nacldir}/curvecp/TARGETS" \
  | sort \
  | while read name
  do
    cd "${naclbuild}/bin"
    cp -p "${name}" "tinysshd-${name}"
  done
)

${naclbuild}/bin/okabi \
| awk '
  { if ($1=="amd64" || $1=="ia64" || $1=="ppc64" || $1=="sparcv9" || $1=="mips64") print 1,$1
    else if ($1 == "mips32") print 2,$1
    else print 3,$1
  }
' \
| sort \
| while read okabipriority abi
do
  ${naclbuild}/bin/okc-${abi} \
  | while read CC
  do
    export CC
    LIBS='-lnacl'; export LIBS
    LDFLAGS="-L${naclbuild}/lib/${abi}"; export LDFLAGS
    CFLAGS="-I${naclbuild}/include/${abi}"; export CFLAGS
    sh -e make-tinyssh.sh && exit 0
  done || exit 111
done || exit 111
exit 0
