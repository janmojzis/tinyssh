#!/bin/sh

(
  (
    echo "CC?=cc"
    echo "CFLAGS+=-W -Wall -Os -fPIC -fwrapv"
    echo "LDFLAGS?="
    echo "CPPFLAGS?="
    echo "DESTDIR?="
    echo 

    # binaries
    i=0
    for file in `ls -1 *.c | grep -v '^has'`; do
      if grep '^int main(' "${file}" >/dev/null; then
        x=`echo "${file}" | sed 's/\.c$//'`
        if [ $i -eq 0 ]; then
          echo "BINARIES=${x}"
        else
          echo "BINARIES+=${x}"
        fi
        i=`expr $i + 1`
      fi
    done
    echo

    echo "all: \$(BINARIES) tinysshd-makekey tinysshd-printkey"
    echo 

    for file in `ls -1 has*.c`; do
      hfile=`echo ${file} | sed 's/\.c/.h/'`
      touch "${hfile}"
    done
    for file in `ls -1 *.c | grep -v '^has'`; do
      (
        gcc -MM "${file}"
        echo "	\$(CC) \$(CFLAGS) \$(CPPFLAGS) -c ${file}"
        echo
      )
    done
    rm has*.h

    i=0
    for file in `ls *.c`; do
      if ! grep '^int main(' "${file}" >/dev/null; then
        x=`echo "${file}" | sed 's/\.c$/.o/'`
        if [ $i -eq 0 ]; then
          echo "OBJECTS=${x}"
        else
          echo "OBJECTS+=${x}"
        fi
        i=`expr $i + 1`
      fi
    done
    echo

    for file in `ls -1 *.c | grep -v '^has'`; do
      if grep '^int main(' "${file}" >/dev/null; then
        x=`echo "${file}" | sed 's/\.c$//'`
        echo "${x}: ${x}.o \$(OBJECTS) libs"
        echo "	\$(CC) \$(CFLAGS) \$(CPPFLAGS) -o ${x} ${x}.o \$(OBJECTS) \$(LDFLAGS) \`cat libs\`"
        echo 
      fi
    done
    echo

    for cfile in `ls -1 has*.c`; do
      hfile=`echo ${cfile} | sed 's/\.c/.h/'`
      touch "${hfile}"
      echo "${hfile}: tryfeature.sh ${cfile} libs"
      echo "	env CC=\"\$(CC)\" CFLAGS=\"\$(CFLAGS)\" LDFLAGS=\"\$(LDFLAGS) \`cat libs\`\" ./tryfeature.sh ${cfile} > ${hfile}"
      echo "	cat ${hfile}"
      echo
    done

    echo "libs: trylib.sh"
    echo "	rm -f libs"
    echo "	env CC=\"\$(CC)\" ./trylib.sh -lsocket -lnsl >>libs"
    echo "	env CC=\"\$(CC)\" ./trylib.sh -lutil >>libs"
    #echo "	env CC=\"\$(CC)\" ./trylib.sh -lrandombytes >>libs"
    #echo "	env CC=\"\$(CC)\" ./trylib.sh -l25519 >>libs"
    echo "	cat libs"
    echo

    echo "tinysshd-makekey: tinysshd"
    echo "	ln -s tinysshd tinysshd-makekey"
    echo

    echo "tinysshd-printkey: tinysshd"
    echo "	ln -s tinysshd tinysshd-printkey"
    echo

    echo "clean:"
    echo "	rm -f *.o has*.h \$(BINARIES) libs tinysshd tinysshd-makekey tinysshd-printkey"
    echo 

  ) > Makefile
)
