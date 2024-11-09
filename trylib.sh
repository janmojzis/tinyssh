#!/bin/sh

# version 20241107

scriptname="`basename $0`"

if [ x"${CC}" = x ]; then
  echo "usage: env CC=<compiler> ${scriptname} -l<library>"
  echo '$CC not set'
  exit 1
fi

if [ x"$1" = x ]; then
  echo "usage: env CC=<compiler> ${scriptname} -l<library>"
  echo 'missing -l<library> argument'
  exit 1
fi

# temporary
name="${scriptname}.`echo "$@" | tr ' ' '_'`.tmp"
tryname="${name}.C"
binname="${tryname}.bin"
logname="${tryname}.log"

cleanup() {
  ex=$?
  rm -f "${binname}" "${tryname}" "${logname}"
  exit "${ex}"
}
trap "cleanup" EXIT TERM INT

cat <<EOF > "${tryname}"
int main(int argc, char **argv) {
    (void) argc;
    (void) argv;
    return 0;
}
EOF

"${CC}" -O0 -o "${binname}" "${tryname}" "$@" 1>"${logname}" 2>&1
if [ $? -eq 0 ]; then
  echo "${scriptname}: '$@' detected" >&2
  echo "$@"
else
  echo "${scriptname}: '$@' not detected" >&2
  cat "${logname}" >&2
fi
exit 0
