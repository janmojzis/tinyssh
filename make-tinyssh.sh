#!/bin/sh -e

top="`pwd`"
build="`pwd`/build"
bin="${build}/bin"
lib="${build}/lib"
include="${build}/include"
log="${build}/log"
work="${build}/work"
contribdir="`pwd`/build-contrib"

rm -rf "${build}"
mkdir -p "${build}"
mkdir -p "${bin}"
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

log1 "checking compiler options"
rm -rf "${work}"
mkdir -p "${work}"
(
  cd "${work}"
  cflags=`cat "${top}/conf-cflags" || :`
  cflags="${CFLAGS} ${LDFLAGS} ${cflags}"

  for i in ${cflags}; do
    echo 'int main(void) { return 0; }' > try.c
    ${compiler} "${i}" -o try try.c || { log2 "${i} failed"; continue; }
    options="${i} ${options}"
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
      ${compiler} -O0 -o "${source}" "${source}.c" ${libs} || continue
      ./${source} > "${target}.tmp" || continue
      cp "${target}.tmp" "${include}/${target}"
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
  done
  ar cr "${lib}/libtinynacl.a" `cat CRYPTOLIBS` || exit 0
)
log2 "libtinynacl.a ok"
log1 "finishing"

origlibs="${origlibs} ${lib}/libtinynacl.a"
libs="${libs} ${lib}/libtinynacl.a"

log1 "starting crypto headers"
rm -rf "${work}"
mkdir -p "${work}"
cp -p crypto/CRYPTOPRIMITIVES "${work}"
cp -pr crypto-tests/*test.c "${work}"
cp -pr crypto-tests/*.h "${work}"
cp -pr crypto-tests/*.data "${work}" 2>/dev/null || :
(
  cd "${work}"
  cat CRYPTOPRIMITIVES\
  | while read primitive checkflag
  do
    if [ "x${checkflag}" = x0 ]; then
      cp -p "${top}/crypto/${primitive}.h" "${include}"
      continue;
    fi
    testf=`echo "${primitive}" | sed 's/$/test/'`
    (
      echo '#include <stdio.h>'
      echo "#include <${primitive}.h>"
      echo 'int main(void) {'
      echo "#ifdef ${primitive}_PRIMITIVE"
      echo "printf(\"%s\\\\n\", ${primitive}_PRIMITIVE);"
      echo '#else'
      echo "#ifdef ${primitive}_IMPLEMENTATION"
      echo "printf(\"%s\\\\n\", ${primitive}_IMPLEMENTATION);"
      echo '#endif'
      echo "#ifdef ${primitive}_implementation"
      echo "printf(\"%s\\\\n\", ${primitive}_implementation);"
      echo '#endif'
      echo '#endif'
      echo 'return 0; }'
    ) > try.c
    #try ext. crypto library
    if ${compiler} -c "${testf}.c" ${libs}; then
      if ${compiler} -o "${testf}" "${testf}.o" ${libs}; then
        if ${compiler} -o try try.c; then
          if /bin/sh -ec "./${testf}"; then
            log2 "${primitive}.h (`./try`) ok"
            echo "#include <${primitive}.h>" >> crypto.h
            continue
          else
            log2 "${primitive}.h (`./try`) failed"
          fi
        fi
      fi
    fi
    #try int. crypto library
    if cp -p "${top}/crypto/${primitive}.h" . ; then
      if ${compilerorig} -I. -I"$include" -o "${testf}" "${testf}.c" ${origlibs}; then
        if ${compilerorig} -I. -I"$include" -o try try.c; then
          if /bin/sh -ec "./${testf}"; then
            log2 "${primitive}.h (`./try`) ok"
            echo "#include \"${primitive}.h\"" >> crypto.h
            cp -p "${primitive}.h" "${include}"
            continue
          fi
        fi
      fi
    fi
    log2 "${primitive}.h failed ... see the log ${log}"
    exit 111
  done
  cp crypto.h "${include}/crypto.h"
)
log1 "finishing"

rm -rf "${work}"
mkdir -p "${work}"
cp -pr tinyssh/* "${work}"
cp -pr tinyssh-tests/*test.c "${work}"
cp -pr tinyssh-tests/*.h "${work}"
cp -pr _tinyssh/* "${work}" 2>/dev/null || :
(
  cd "${work}"
  log1 "starting tinyssh objects"
  touch SOURCES TARGETS _TARGETS
  cat SOURCES TARGETS _TARGETS\
  | while read x
  do
    ${compiler} "-DCOMPILER=\"${compilerorig}\"" "-DVERSION=\"${version}\"" -I"${include}" -c "${x}.c" || { log2 "${x}.o failed ... see the log ${log}"; exit 111; }
    log2 "${x}.o ok"
  done
  ar cr libtinyssh.a `cat LIBS` || exit 111
  log1 "finishing"

  #tests
  log1 "starting tinyssh-tests"
  cat LIBS \
  | while read x
  do
    t=`echo ${x} | sed 's/.o$/test/'`
    if [ ! -h "${t}.c" ]; then
      ${compiler} -I"${include}" -c "${t}.c" || { log2 "${t} failed ... see the log ${log}"; exit 111; }
      ${compiler} -I"${include}" -o "${t}" "${t}.o" libtinyssh.a ${libs} || { log2 "${t} failed ... see the log ${log}"; exit 111; }
      "./${t}" || { log2 "${t} failed ... see the log ${log}"; exit 111; }
      log2 "${t} ok"
    fi
  done || exit 111
  log1 "finishing"

  log1 "starting _tinyssh"
  cat _TARGETS \
  | while read x
  do
    ${compiler} -I"${include}" -o "${x}" "${x}.o" libtinyssh.a ${libs} || { log2 "${x} failed ... see the log"; exit 111; }
    log2 "${x} ok"
    cp -p "${x}" "${bin}/${x}";
  done
  log1 "finishing"

  log1 "starting tinyssh"
  cat TARGETS \
  | while read x
  do
    ${compiler} -I"${include}" -o "${x}" "${x}.o" libtinyssh.a ${libs} || { log2 "${x} failed ... see the log"; exit 111; }
    log2 "${x} ok"
  done
  log1 "finishing"

  log1 "starting tinyssh regression tests"
  cat TARGETS \
  | while read x
  do
    sh ${x}.rts > ${x}.out
    cmp "${x}.out" "${x}.exp" || { log2 "${x} regression test failed ... see the difference `pwd`/${x}.out `pwd`/${x}.exp"; exit 111; }
    log2 "${x} ok"
    cp -p "${x}" "${bin}/${x}";
  done || exit 111
  log1 "finishing"

) || exit 111


log1 "starting curvecp"
rm -rf "$work"
mkdir -p "$work"
cp -pr curvecp/* "$work"
(
  cd "$work"
  cat SOURCES\
  | while read x
  do
    ${compiler} -I"${include}" -c "$x.c" || exit 111
  done
  ar cr libcurvecp.a `cat LIBS` || exit 111

  cat TARGETS \
  | while read x y
  do
    ${compiler} -I"${include}" -o "${x}" "${x}.o" libcurvecp.a ${libs} || exit 111
    log2 "${y} ok"
    cp -p "${x}" "${bin}/${y}";
  done
  log1 "finishing"
) || exit 111


log1 "counting words of code - tests"
rm -rf "${work}"
mkdir -p "${work}"

for dir in tinyssh-tests crypto-tests _tinyssh; do
  (
    touch "${work}/${dir}"
    [ -d "${dir}" ] || exit 0
    cd "${dir}" 
    cat *.c *.h > "${work}/${dir}" || :
  )

  (
    cd "${work}"
    cat "${dir}" \
    | (
      cpp -fpreprocessed || gcpp -fpreprocessed
    ) | (
      x=`sed 's/[_a-zA-Z0-9][_a-zA-Z0-9]*/x/g' | tr -d ' \012' | wc -c | tr -d ' '`
      log2 "${dir} ${x}"
    )
  )
done
(
  cd "${work}"
  cat * \
  | (
    cpp -fpreprocessed || gcpp -fpreprocessed
  ) | (
    x=`sed 's/[_a-zA-Z0-9][_a-zA-Z0-9]*/x/g' | tr -d ' \012' | wc -c | tr -d ' '`
    log2 "$x words of code"
  )
)
log1 "finishing"


echo "=== `date` === counting words of code"
rm -rf "${work}"
mkdir -p "${work}"

for dir in sysdep tinyssh crypto; do
  (
    cd "${dir}"
    cat *.c *.h > "${work}/${dir}" || :
  )

  (
    cd "${work}"
    cat "${dir}" \
    | (
      cpp -fpreprocessed || gcpp -fpreprocessed
    ) | (
      x=`sed 's/[_a-zA-Z0-9][_a-zA-Z0-9]*/x/g' | tr -d ' \012' | wc -c | tr -d ' '`
      log2 "${dir} ${x}"
    )
  )
done
(
  cd "${work}"
  cat * \
  | (
    cpp -fpreprocessed || gcpp -fpreprocessed
  ) | (
    x=`sed 's/[_a-zA-Z0-9][_a-zA-Z0-9]*/x/g' | tr -d ' \012' | wc -c | tr -d ' '`
    log2 "${x} words of code"
  )
)
log1 "finishing"
exit 0
