#!/bin/sh

# version 20241109

scriptname="`basename $0`"

if [ x"${CC}" = x ]; then
  echo "usage: env CC=<compiler> ${scriptname} -l<library> -l<library> ..."
  echo '$CC not set'
  exit 1
fi

if [ x"$1" = x ]; then
  echo "usage: env CC=<compiler> ${scriptname} -l<library> -l<library> ..."
  echo 'missing -l<library> argument'
  exit 1
fi

# temporary
name="${scriptname}"
tryname="${name}.tmp.C"
binname="${tryname}.tmp.bin"
logname="${tryname}.tmp.log"

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

while true; do
  [ x"$1" = x ] && break
  "${CC}" -O0 -o "${binname}" "${tryname}" "$1" 1>"${logname}" 2>&1
  if [ $? -eq 0 ]; then
    echo "${scriptname}: '$1' detected" >&2
    echo "$1"
  else
    echo "${scriptname}: '$1' not detected" >&2
    cat "${logname}" >&2
  fi
  shift
done
exit 0
