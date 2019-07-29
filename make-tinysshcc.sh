#!/bin/sh -e

top="`pwd`"
build="`pwd`/build"
bin="${build}/bin"
man="${build}/man"
lib="${build}/lib"
include="${build}/include"
log="${build}/log"
work="${build}/work"
contribdir="`pwd`/build-contrib"

rm -rf "${build}"
mkdir -p "${build}"
mkdir -p "${bin}"
mkdir -p "${man}"
mkdir -p "${lib}"
mkdir -p "${include}"

exec 5>"${log}"
exec 2>&5
exec </dev/null

log0() {
  echo "=== `date` === $@" >&5
}

log1() {
  echo "=== `date` === $@"
  echo "=== `date` === $@" >&5
}

log2() {
  echo "=== `date` ===   $@"
  echo "=== `date` ===   $@" >&5
}

version=`head -1 "${top}/debian/changelog"  | cut -d '(' -f2 | cut -d ')' -f1` #XXX
if [ x"${version}" = x ]; then
  version=noversion
fi

LANG=C
export LANG

log0 "uname -a: `uname -a || :`"
log0 "uname -F: `uname -F || :`"
log0 "uname -M: `uname -M || :`"

log1 "obtaining compiler"
rm -rf "${work}"
mkdir -p "${work}"
(
  cd "${work}"
  (
    if [ x"${CC}" != x ]; then
      echo "${CC} "
    fi
    cat "${top}/conf-cc"
  ) | while read compiler
  do
    echo 'int main(void) { return 0; }' > try.c
    ${compiler} -o try try.c || { log2 "${compiler} failed"; continue; }
    log2 "${compiler} ok"
    echo "${compiler}" > compiler
    break
  done
)
compiler=`head -1 "${work}/compiler"`
log1 "finishing"

log1 "obtaining ar"
rm -rf "${work}"
mkdir -p "${work}"
(
  cd "${work}"
  (
    if [ x"${AR}" != x ]; then
      echo "${AR} "
    fi
    cat "${top}/conf-ar"
  ) | while read ar
  do
    touch test.o
    ${ar} cr test.a test.o || { log2 "${ar} failed"; continue; }
    log2 "${ar} ok"
    echo "${ar}" > ar
    break
  done
)
ar=`head -1 "${work}/ar"`
log1 "finishing"

log1 "checking compiler options"
rm -rf "${work}"
mkdir -p "${work}"
(
  cd "${work}"
  cflags=`cat "${top}/conf-cflags" || :`
  cflags="${CPPFLAGS} ${CFLAGS} ${LDFLAGS} ${cflags}"

  for i in ${cflags}; do
    echo 'int main(void) { return 0; }' > try.c
    ${compiler} ${options} "${i}" -o try try.c || { log2 "${i} failed"; continue; }
    options="${options} ${i}"
    log2 "${i} ok"
  done
  echo ${options} > options
)
compilerorig=${compiler}
compiler="${compiler} `cat ${work}/options`"
log2 "${compiler}"
log1 "finishing"

log1 "checking libs"
rm -rf "${work}"
mkdir -p "${work}"
(
  cd "${work}"
  (
    cat "${top}/conf-libs" || :
  ) | (
    exec 9>syslibs
    while read i; do
      echo 'int main(void) { return 0; }' > try.c
      ${compiler} ${i} -o try try.c || { log2 "${i} failed"; continue; }
      echo "${i}" >&9
      log2 "${i} ok"
    done
  )
)
libs=`cat "${work}/syslibs"`
log1 "finishing"

log1 "checking \$LIBS"
if [ x"${LIBS}" != x ]; then
  rm -rf "${work}"
  mkdir -p "${work}"
  (
    cd "${work}"
    for i in ${LIBS}; do
      echo 'int main(void) { return 0; }' > try.c
      ${compiler} ${i} -o try try.c || { log2 "${i} failed"; continue; }
      syslibs="${i} ${syslibs}"
      log2 "${i} ok"
    done
    echo ${syslibs} > syslibs
 )
fi
libsorig=${libs}
libs="${libs} `cat ${work}/syslibs`"
log1 "finishing"

log1 "building sysdep headers"
rm -rf "${work}"
mkdir -p "${work}"
cp -pr sysdep/* "${work}"
(
  cd "${work}"
  sh list | (
    while read target source
    do
      [ -f "${include}/${target}" ] && continue
      rm -f "${source}" "${target}.tmp"
      [ -f "${source}.out" ] || continue
      ${compiler} -O0 -o "${source}" "${source}.c" ${libs} || continue
      cp "${source}.out" "${include}/${target}"
      log2 "${target} ${source}"
    done
  )
)
log1 "finishing"

log1 "starting crypto lib"
rm -rf "${work}"
mkdir -p "${work}"
cp -pr crypto/* "${work}"
(
  cd "${work}"
  cat CRYPTOSOURCES\
  | while read x
  do
    ${compiler} -I"${include}" -c "${x}.c" || { log2 "libtinynacl.a failed ... see the log ${log}"; exit 111; }
  done || exit 111
  ${ar} cr "${lib}/libtinynacl.a" `cat CRYPTOLIBS` || exit 0
)
log2 "libtinynacl.a ok"
log1 "finishing"

origlibs="${origlibs} ${lib}/libtinynacl.a"
libs="${libs} ${lib}/libtinynacl.a"

log1 "starting crypto headers"
rm -rf "${work}"
mkdir -p "${work}"
cp -p crypto/CRYPTOPRIMITIVES "${work}"
(
  cd "${work}"
  cat CRYPTOPRIMITIVES\
  | while read primitive checkflag
  do
    if [ "x${checkflag}" != x0 ]; then
      echo "#include \"${primitive}.h\"" >> crypto.h
    fi
    cp -p "${top}/crypto/${primitive}.h" "${include}"
  done || exit 111
  cp crypto.h "${include}/crypto.h"
)
log1 "finishing"

rm -rf "${work}"
mkdir -p "${work}"
cp -pr tinyssh/* "${work}"
(
  cd "${work}"
  log1 "starting tinyssh objects"
  touch SOURCES TARGETS
  cat SOURCES TARGETS\
  | while read x
  do
    ${compiler} "-DCOMPILER=\"${compilerorig}\"" "-DVERSION=\"${version}\"" -I"${include}" -c "${x}.c" || { log2 "${x}.o failed ... see the log ${log}"; exit 111; }
    log2 "${x}.o ok"
  done || exit 111
  ${ar} cr libtinyssh.a `cat LIBS` || exit 111
  log1 "finishing"

  log1 "starting tinyssh"
  cat TARGETS \
  | while read x
  do
    ${compiler} -I"${include}" -o "${x}" "${x}.o" libtinyssh.a ${libs} || { log2 "${x} failed ... see the log ${log}"; exit 111; }
    cp -p "${x}" "${bin}/${x}"
    log2 "${x} ok"
  done || exit 111
  log1 "finishing"

) || exit 111

log1 "starting manpages"
cp -pr man/* "${man}"
log1 "finishing"

exit 0
