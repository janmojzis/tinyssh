#!/bin/sh

rm -rf keydir
mkdir keydir


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
    echo ${key} > "keydir/${name}"
  done
)

echo '--- tinysshd-printkey prints keys'
echo
./tinysshd-printkey keydir
echo $?
echo

mv keydir/nistp256ecdsa.pk keydir/nistp256ecdsa.pk.bk

echo '--- tinysshd-printkey prints only ssh-ed25519 key'
echo
./tinysshd-printkey keydir
echo $?
echo

mv keydir/nistp256ecdsa.pk.bk keydir/nistp256ecdsa.pk
mv keydir/ed25519.pk keydir/ed25519.pk.bk

echo '--- tinysshd-printkey prints only ecdsa-sha2-nistp256 key'
echo
./tinysshd-printkey keydir
echo $?
echo


./tinysshd-makekey 2>/dev/null || :

echo "--- tinysshd-printkey handles '-r' parameter (prints help)"
echo
./tinysshd-printkey -r 2>/dev/null
echo $?
echo

echo "--- tinysshd-printkey prints keys from '-r' key-directory"
echo
./tinysshd-printkey -- -r 1>/dev/null
echo $?
echo

exit 0
