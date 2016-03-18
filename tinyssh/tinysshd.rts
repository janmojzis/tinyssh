#!/bin/sh

LANG=C
export LANG

LC_ALL=C
export LC_ALL

exec 2>&1

rm -rf keydir
./tinysshd-makekey keydir

echo "--- tinysshd don't accept long hello message"
echo
echo 'SSH-2.0 xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx' | ./tinysshd keydir 1>/dev/null 2>/dev/null
echo $?
echo

echo "--- tinysshd don't accept short hello message"
echo
echo 'SSH' | ./tinysshd keydir 1>/dev/null 2>/dev/null
echo $?
echo

echo "--- tinysshd don't accept lower case SSH in hello message"
echo
echo 'ssh-2.0 xxx' | ./tinysshd keydir 1>/dev/null 2>/dev/null
echo $?
echo

echo "--- tinysshd don't accept too many unauthenticated messages"
echo
./_tinysshd-unauthenticated ./tinysshd keydir
echo $?
echo

echo '--- tinysshd default setup'
echo 
./_tinysshd-printkex ./tinysshd keydir
echo 

for i in o O; do
  for j in s S; do
    for k in p P; do
      echo "--- tinysshd recognizes -${i}${j}${k}"
      echo
      ./_tinysshd-printkex ./tinysshd "-${i}${j}${k}" keydir
      echo
      echo "--- tinysshd recognizes -${i} -${j} -${k}"
      echo
      ./_tinysshd-printkex ./tinysshd "-${i}" "-${j}" "-${k}" keydir
      echo
    done
  done
done

rm -rf keydir
./tinysshd-makekey keydir
rm -f keydir/nistp256ecdsa.pk

for i in o O; do
  for j in s S; do
    for k in p P; do
      echo "--- tinysshd recognizes -${i}${j}${k}, ecdsa-sha2-nistp256 key missing"
      echo
      ./_tinysshd-printkex ./tinysshd "-${i}${j}${k}" keydir
      echo
      echo "--- tinysshd recognizes -${i} -${j} -${k}, ecdsa-sha2-nistp256 key missing"
      echo
      ./_tinysshd-printkex ./tinysshd "-${i}" "-${j}" "-${k}" keydir
      echo
    done
  done
done

rm -rf keydir
./tinysshd-makekey keydir
rm -f keydir/ed25519.pk

for i in o O; do
  for j in s S; do
    for k in p P; do
      echo "--- tinysshd recognizes -${i}${j}${k}, ssh-ed25519 key missing"
      echo ""
      ./_tinysshd-printkex ./tinysshd "-${i}${j}${k}" keydir
      echo
      echo "--- tinysshd recognizes -${i} -${j} -${k}, ssh-ed25519 key missing"
      echo
      ./_tinysshd-printkex ./tinysshd "-${i}" "-${j}" "-${k}" keydir
      echo
    done
  done
done

#temporary removed
exit 0

rm -rf keydir
./tinysshd-makekey keydir

echo "--- tinysshd logs 'client closed connection' when trying to write hello message"
echo ""
./_tinysshd-test-hello1 ./tinysshd keydir  2>&1 | sed -e 's/tinysshd: .*: //' -e 's/SSH-.*//' -e 's/{.*}//'
echo ""

echo "--- tinysshd logs 'client closed the connection' when trying to read hello message"
echo ""
./_tinysshd-test-hello2 '' ./tinysshd keydir  2>&1 | sed -e 's/tinysshd: .*: //' -e 's/SSH-.*//' -e 's/{.*}//'
echo ""

echo "--- tinysshd 'logs unacceptable hello string' when trying to read hello message"
echo ""
./_tinysshd-test-hello2 SSH ./tinysshd keydir  2>&1 | sed -e 's/tinysshd: .*: //' -e 's/SSH-.*//' -e 's/{.*}//'
echo ""

echo "--- tinysshd 'logs unacceptable hello string' when trying to read hello message"
echo ""
./_tinysshd-test-hello2 ssh- ./tinysshd keydir  2>&1 | sed -e 's/tinysshd: .*: //' -e 's/SSH-.*//' -e 's/{.*}//'
echo ""

echo "--- tinysshd 'logs unacceptable hello string' when trying to read long hello message"
echo ""
./_tinysshd-test-hello2  'SSH-2.0 xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx' ./tinysshd -v keydir  2>&1 | sed -e 's/tinysshd: .*: //' -e 's/SSH-.*//' -e 's/{.*}//'
echo ""

echo "--- tinysshd 'logs kex problem' when client closed the connection - ends with (connection reset) or (broken pipe)"
echo ""
./_tinysshd-test-kex1 ./tinysshd keydir  2>&1 | sed -e 's/tinysshd: .*: //' -e 's/{.*}//' -e 's/(.*)//'
echo ""

echo "--- tinysshd 'logs kex problem' when client disconnected the connection"
echo ""
./_tinysshd-test-kex2 ./tinysshd keydir  2>&1 | sed -e 's/tinysshd: .*: //' -e 's/{.*}//'
echo ""

exit 0
