#!/bin/sh

(
  (
    echo "CC?=cc"
    echo "CFLAGS+=-W -Wall -Os -fPIC -fwrapv -Werror=deprecated-declarations"
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

    echo "all: \$(BINARIES) tinysshd-makekey tinysshd-printkey tinysshnoneauthd"
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
      lfile=`echo ${cfile} | sed 's/\.c/.log/'`
      touch "${hfile}"
      echo "${hfile}: tryfeature.sh ${cfile} libs"
      echo "	env CC=\"\$(CC)\" CFLAGS=\"\$(CFLAGS)\" LDFLAGS=\"\$(LDFLAGS) \`cat libs\`\" ./tryfeature.sh ${cfile} >${hfile} 2>${lfile}"
      echo "	cat ${hfile}"
      echo
    done

    echo "libs: trylibs.sh"
    echo "	env CC=\"\$(CC)\" ./trylibs.sh -lsocket -lnsl -lutil -lrandombytes -l25519 -lntruprime >libs"
    echo "	cat libs"
    echo

    echo "tinysshd-makekey: tinysshd"
    echo "	ln -s tinysshd tinysshd-makekey"
    echo

    echo "tinysshd-printkey: tinysshd"
    echo "	ln -s tinysshd tinysshd-printkey"
    echo

    echo "tinysshnoneauthd: tinysshd"
    echo "	ln -s tinysshd tinysshnoneauthd"
    echo

    echo "install: \$(BINARIES) tinysshd-makekey tinysshd-printkey tinysshnoneauthd"
    echo "	mkdir -p \$(DESTDIR)/usr/sbin"
    echo "	install -m 0755 tinysshd \$(DESTDIR)/usr/sbin/tinysshd"
    echo "	install -m 0755 tinysshd-makekey \$(DESTDIR)/usr/sbin/tinysshd-makekey"
    echo "	install -m 0755 tinysshd-printkey \$(DESTDIR)/usr/sbin/tinysshd-printkey"
    echo "	install -m 0755 tinysshnoneauthd \$(DESTDIR)/usr/sbin/tinysshnoneauthd"
    echo "	mkdir -p \$(DESTDIR)/usr/share/man/man8"
    echo "	install -m 0644 man/tinysshd.8 \$(DESTDIR)/usr/share/man/man8/tinysshd.8"
    echo "	install -m 0644 man/tinysshd-makekey.8 \$(DESTDIR)/usr/share/man/man8/tinysshd-makekey.8"
    echo "	install -m 0644 man/tinysshd-printkey.8 \$(DESTDIR)/usr/share/man/man8/tinysshd-printkey.8"
    echo "	install -m 0644 man/tinysshnoneauthd.8 \$(DESTDIR)/usr/share/man/man8/tinysshnoneauthd.8"
    echo

    echo "test: \$(BINARIES) tinysshd-makekey tinysshd-printkey tinysshnoneauthd"
    echo "	sh runtest.sh test-tinysshd.sh test-tinysshd.out test-tinysshd.exp"
    echo "	sh runtest.sh test-tinysshd-makekey.sh test-tinysshd-makekey.out test-tinysshd-makekey.exp"
    echo "	sh runtest.sh test-tinysshd-printkey.sh test-tinysshd-printkey.out test-tinysshd-printkey.exp"
    echo "	sh runtest.sh test-tinysshnoneauthd.sh test-tinysshnoneauthd.out test-tinysshnoneauthd.exp"
    echo

    echo "clean:"
    echo "	rm -f *.o *.out *.log has*.h \$(BINARIES) libs tinysshd tinysshd-makekey tinysshd-printkey tinysshnoneauthd"
    echo 

  ) > Makefile
)
