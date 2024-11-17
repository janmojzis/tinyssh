#!/bin/sh

LANG=C
export LANG

LC_ALL=C
export LC_ALL

cleanup() {
  ex=$?
  rm -rf -- keydirm -m
  exit "${ex}"
}
trap "cleanup" EXIT TERM INT

echo '--- tinysshd-makekey prints help'
echo
./tinysshd-makekey 2>/dev/null
echo $?
echo

echo '--- tinysshd-makekey creates key-directory'
echo
rm -rf keydirm
./tinysshd-makekey keydirm 2>&1
ls keydirm/*.pk | sort
echo $?
echo

echo '--- tinysshd-makekey fails when directory exists'
echo
rm -rf keydirm
mkdir keydirm
./tinysshd-makekey keydirm 2>&1
echo $?

echo "--- tinysshd-makekey handles '-m' parameter (prints help)"
echo
./tinysshd-makekey -m 2>/dev/null
echo $?
echo

echo "--- tinysshd-makekey creates '-m' key-directory"
echo
./tinysshd-makekey -- -m 2>&1
echo $?
echo
