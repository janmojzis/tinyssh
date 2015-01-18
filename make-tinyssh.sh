#!/bin/sh -e

top="`pwd`"
build="`pwd`/build"
bin="${build}/bin"
lib="$build/lib"
include="$build/include"
work="$build/work"
contribdir="`pwd`/build-contrib"

rm -rf "$build"
mkdir -p "$build"
mkdir -p "$bin"
mkdir -p "$lib"
mkdir -p "$include"

version=`head -1 "${top}/debian/changelog"  | cut -d '(' -f2 | cut -d ')' -f1` #XXX

LANG=C
export LANG

echo "=== `date` === obtaining compiler"
rm -rf "$work"
mkdir -p "$work"
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
    ${compiler} -o try try.c 2>/dev/null || { echo "=== `date` ===   ${compiler} failed"; continue; }
    echo "=== `date` ===   ${compiler} ok"
    echo "${compiler}" > compiler
    break
  done
)
compiler=`head -1 "${work}/compiler"`
echo "=== `date` === finishing"

echo "=== `date` === checking compiler options"
rm -rf "$work"
mkdir -p "$work"
(
  cd "${work}"
  #TODO -Wstrict-prototypes -Wwrite-strings -Wunused-parameter -Wunused-value -Wmissing-prototypes -Wmissing-declarations
  for i in  -Wundef \
            -Wdeclaration-after-statement \
            -Wshadow \
            -Wno-unused-function \
            -Wno-sign-compare \
            -Wno-overlength-strings \
            -Wno-deprecated-declarations \
            -Wno-long-long -Wall \
            -pedantic ${CFLAGS} ${LDFLAGS}; do
    echo 'int main(void) { return 0; }' > try.c
    ${compiler} "$i" -o try try.c 2>/dev/null || { echo "=== `date` ===   $i failed"; continue; }
    options="$i $options"
    echo "=== `date` ===   $i ok"
  done
  echo $options > options
)
compilerorig=${compiler}
compiler="${compiler} `cat ${work}/options`"
echo "=== `date` ===   $compiler"
echo "=== `date` === finishing"

echo "=== `date` === checking libs"
rm -rf "$work"
mkdir -p "$work"
(
  cd "$work"
  for i in '-lutil' '-lsocket -lnsl'; do
    echo 'int main(void) { return 0; }' > try.c
    ${compiler} $i -o try try.c 2>/dev/null || { echo "=== `date` ===   $i failed"; continue; }
    syslibs="$i $syslibs"
    echo "=== `date` ===   $i ok"
  done
  echo $syslibs > syslibs
)
libs=`cat "${work}/syslibs"`
echo "=== `date` === finishing"

echo "=== `date` === checking \$LIBS"
if [ x"${LIBS}" != x ]; then
  rm -rf "$work"
  mkdir -p "$work"
  (
    cd "$work"
    for i in ${LIBS}; do
      echo 'int main(void) { return 0; }' > try.c
      ${compiler} $i -o try try.c 2>/dev/null || { echo "=== `date` ===   $i failed"; continue; }
      syslibs="$i $syslibs"
      echo "=== `date` ===   $i ok"
    done
    echo $syslibs > syslibs
 )
fi
libsorig=${libs}
libs="${libs} `cat ${work}/syslibs`"
echo "=== `date` === finishing"


echo "=== `date` === building sysdep headers"
rm -rf "$work"
mkdir -p "$work"
cp -pr sysdep/* "$work"
(
  cd "$work"
  sh list | (
    while read target source
    do
      [ -f "${include}/${target}" ] && continue
      rm -f "$source" "$target.tmp" 
      ${compiler} -O0 -o "$source" "$source.c" $libs 2>/dev/null || continue
      ./$source > "$target.tmp" 2>/dev/null || continue
      cp "$target.tmp" "$include/$target"
      echo "=== `date` ===   $target $source"
    done
  )
)
echo "=== `date` === finishing"

echo "=== `date` === starting crypto lib"
rm -rf "$work"
mkdir -p "$work"
cp -pr crypto/* "$work"
(
  cd "$work"
  cat CRYPTOSOURCES\
  | while read x
  do
    $compiler -I"$include" -c "$x.c" || exit 111
    #echo "=== `date` ===   $x.o ok"
  done
  ar cr "${lib}/libtinynacl.a" `cat CRYPTOLIBS` || exit 0
  #cp *.h "${include}"
)
echo "=== `date` ===   libtinynacl.a ok"
echo "=== `date` === finishing"

origlibs="${origlibs} ${lib}/libtinynacl.a"
libs="${libs} ${lib}/libtinynacl.a"

echo "=== `date` === starting crypto headers"
rm -rf "$work"
mkdir -p "$work"
cp -p crypto/CRYPTOPRIMITIVES "$work"
cp -pr crypto-tests/*test.c "$work"
cp -pr crypto-tests/*.h "$work"
cp -pr crypto-tests/*.data "$work"
(
  cd "$work"
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
    if ${compiler} -c "${testf}.c" ${libs} 2>try.log; then
      if ${compiler} -o "${testf}" "${testf}.o" ${libs}; then
        if ${compiler} -o try try.c 2>try.log; then
          if /bin/sh -ec "./${testf}" 2>try.log; then
            echo "=== `date` ===   ${primitive}.h (`./try`) ok"
            echo "#include <${primitive}.h>" >> crypto.h
            continue
          else
            cat try.log
            echo "=== `date` ===   ${primitive}.h (`./try`) failed"
          fi
        fi
      fi
    fi
    #try int. crypto library
    if cp -p "${top}/crypto/${primitive}.h" . 2>try.log; then
      if ${compilerorig} -I. -I"$include" -o "${testf}" "${testf}.c" ${origlibs} 2>try.log; then
        if ${compilerorig} -I. -I"$include" -o try try.c 2>try.log; then
          if /bin/sh -ec "./${testf}" 2>try.log; then
            echo "=== `date` ===   ${primitive}.h (`./try`) ok"
            echo "#include \"${primitive}.h\"" >> crypto.h
            cp -p "${primitive}.h" "${include}"
            continue
          fi
        fi
      fi
    fi
    cat try.log
    echo "=== `date` ===   ${primitive}.h failed"
    exit 111
  done
  cp crypto.h "$include/crypto.h"
)
echo "=== `date` === finishing"

rm -rf "$work"
mkdir -p "$work"
cp -pr tinyssh/* "$work"
cp -pr tinyssh-tests/*test.c "$work"
cp -pr tinyssh-tests/*.h "$work"
cp -pr crypto/fep256.h "$work"
cp -pr crypto/fe25519.h "$work"
cp -pr crypto/fe.h "$work"
cp -pr _tinyssh/* "$work"
(
  cd "$work"
  cat SOURCES TARGETS _TARGETS\
  | while read x
  do
    $compiler "-DCOMPILER=\"${compilerorig}\"" "-DVERSION=\"${version}\"" -I"$include" -c "$x.c" || exit 111
    #echo "=== `date` ===   $x.o ok"
  done
  ar cr libtinyssh.a `cat LIBS` || exit 111

  #tests
  echo "=== `date` === starting tinyssh-tests"
  cat LIBS \
  | while read x
  do
    t=`echo ${x} | sed 's/.o$/test/'`
    if [ ! -h "${t}.c" ]; then
      $compiler -I"$include" -c "$t.c" || exit 111
      $compiler -I"$include" -o "${t}" "${t}.o" libtinyssh.a $libs || exit 111
      "./$t" || ( echo "=== `date` ===   $t FAILED !!!!!!!!!!!!!!!!!!!!!!!"; exit 111; ) || exit 111
      echo "=== `date` ===   $t ok"
    fi
  done || exit 111
  echo "=== `date` === finishing"

  #echo "=== `date` ===   libtinyssh.a ok"

  echo "=== `date` === starting _tinyssh"
  cat _TARGETS \
  | while read x
  do
    $compiler -I"$include" -o "$x" "$x.o" libtinyssh.a $libs || exit 111
    echo "=== `date` ===   $x ok"
    cp -p "$x" "$bin/$x";
  done
  echo "=== `date` === finishing"

  echo "=== `date` === starting tinyssh"
  cat TARGETS \
  | while read x
  do
    $compiler -I"$include" -o "$x" "$x.o" libtinyssh.a $libs || exit 111
    echo "=== `date` ===   $x ok"
  done
  echo "=== `date` === finishing"

  echo "=== `date` === starting tinyssh regression tests"
  cat TARGETS \
  | while read x
  do
    sh ${x}.rts > ${x}.out
    cmp "${x}.out" "${x}.exp" || { echo "=== `date` ===   ${x} TEST FAILED !!!!!!!!!!!!!!!!!!!!!!!"; diff "${x}.out" "${x}.exp"; exit 111; }
    echo "=== `date` ===   $x ok"
    cp -p "$x" "$bin/$x";
  done || exit 111
  echo "=== `date` === finishing"

) || exit 111


echo "=== `date` === counting words of code - tests"
rm -rf "$work"
mkdir -p "$work"

for dir in tinyssh-tests crypto-tests _tinyssh; do
  (
    cd "${dir}"
    cat *.c *.h > "${work}/${dir}"
  )

  (
    cd "${work}"
    cat "${dir}" \
    | (
      exec 2>/dev/null;
      cpp -fpreprocessed || gcpp -fpreprocessed
    ) | (
      x=`sed 's/[_a-zA-Z0-9][_a-zA-Z0-9]*/x/g' | tr -d ' \012' | wc -c | tr -d ' '`
      echo "=== `date` ===   ${dir} $x"
    )
  )
done
(
  cd "${work}"
  cat * \
  | (
    exec 2>/dev/null;
    cpp -fpreprocessed || gcpp -fpreprocessed
  ) | (
    x=`sed 's/[_a-zA-Z0-9][_a-zA-Z0-9]*/x/g' | tr -d ' \012' | wc -c | tr -d ' '`
    echo "=== `date` ===   $x words of code"
  )
)
echo "=== `date` === finishing"


echo "=== `date` === counting words of code"
rm -rf "$work"
mkdir -p "$work"

for dir in sysdep tinyssh crypto; do
  (
    cd "${dir}"
    cat *.c *.h > "${work}/${dir}"
  )

  (
    cd "${work}"
    cat "${dir}" \
    | (
      exec 2>/dev/null; 
      cpp -fpreprocessed || gcpp -fpreprocessed
    ) | (
      x=`sed 's/[_a-zA-Z0-9][_a-zA-Z0-9]*/x/g' | tr -d ' \012' | wc -c | tr -d ' '`
      echo "=== `date` ===   ${dir} $x"
    )
  )
done
(
  cd "${work}"
  cat * \
  | (
    exec 2>/dev/null;
    cpp -fpreprocessed || gcpp -fpreprocessed
  ) | (
    x=`sed 's/[_a-zA-Z0-9][_a-zA-Z0-9]*/x/g' | tr -d ' \012' | wc -c | tr -d ' '`
    echo "=== `date` ===   $x words of code"
  )
)
echo "=== `date` === finishing"
exit 0
