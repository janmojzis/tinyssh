#!/bin/sh

cleanup() {
  ex=$?
  rm -rf -- keydirp -r
  exit "${ex}"
}
trap "cleanup" EXIT TERM INT

mkdir keydirp

echo '--- tinysshd-printkey prints help'
echo
./tinysshd-printkey 2>/dev/null
echo $?
echo

(
  echo 'ed25519.pk xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx'
  echo 'nistp256ecdsa.pk xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx'
) | (
  while read name key
  do
    echo ${key} > "keydirp/${name}"
  done
)

echo '--- tinysshd-printkey prints keys'
echo
./tinysshd-printkey keydirp
echo $?
echo

mv keydirp/nistp256ecdsa.pk keydirp/nistp256ecdsa.pk.bk

echo '--- tinysshd-printkey prints only ssh-ed25519 key'
echo
./tinysshd-printkey keydirp
echo $?
echo

mv keydirp/nistp256ecdsa.pk.bk keydirp/nistp256ecdsa.pk
mv keydirp/ed25519.pk keydirp/ed25519.pk.bk

echo '--- tinysshd-printkey prints only ecdsa-sha2-nistp256 key'
echo
./tinysshd-printkey keydirp
echo $?
echo


echo "--- tinysshd-printkey handles '-r' parameter (prints help)"
echo
./tinysshd-printkey -r 2>/dev/null
echo $?
echo

./tinysshd-makekey -- -r 2>/dev/null || :

echo "--- tinysshd-printkey prints keys from '-r' key-directory"
echo
./tinysshd-printkey -- -r 1>/dev/null
echo $?
echo

exit 0
