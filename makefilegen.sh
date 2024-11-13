#!/bin/sh

LANG=C
export LANG

(
  (
    echo "CC?=cc"
    echo "CFLAGS+=-W -Wall -Os -fPIC -fwrapv -Werror=deprecated-declarations"
    echo "LDFLAGS?="
    echo "CPPFLAGS?="
    echo
    echo "DESTDIR?="
    echo "PREFIX?=/usr/local"
    echo
    echo "INSTALL?=install"
    echo

    # binaries + objects
    binaries=''
    binobj=''
    for cfile in `ls -1 *.c | sort | grep -v '^has'`; do
      if grep '^int main(' "${cfile}" >/dev/null; then
        binaries="${binaries} `echo ${cfile} | sed 's/\.c$//'`"
        binobj="${binobj} `echo ${cfile} | sed 's/\.c$/.o/'`"
      else
        objects="${objects} `echo ${cfile} | sed 's/\.c$/.o/'`"
      fi
    done
    echo "`echo BINARIES=${binaries} | fold -s | sed 's/^/ /' | sed 's/^ BINARIES= /BINARIES=/' | sed 's/ $/ \\\\/'`"
    echo

    # links
    echo "LINKS=tinysshd-makekey tinysshd-printkey tinysshnoneauthd"
    echo

    echo "`echo OBJECTS=${objects} | fold -s | sed 's/^/ /' | sed 's/^ OBJECTS= /OBJECTS=/' | sed 's/ $/ \\\\/'`"
    echo

    echo "`echo BINOBJ=${binobj} | fold -s | sed 's/^/ /' | sed 's/^ BINOBJ= /BINOBJ=/' | sed 's/ $/ \\\\/'`"
    echo

    # autoheaders
    autoheaders=''
    for cfile in `ls -1 has*.c | sort`; do
      autoheaders="${autoheaders} `echo ${cfile} | sed 's/\.c/.h/'`"
    done
    echo "`echo AUTOHEADERS=${autoheaders} | fold -s | sed 's/^/ /' | sed 's/^ AUTOHEADERS= /AUTOHEADERS=/' | sed 's/ $/ \\\\/'`"
    echo

    echo "all: \$(AUTOHEADERS) \$(BINARIES) \$(LINKS)"
    echo

    for cfile in `ls -1 has*.c | sort`; do
      hfile=`echo ${cfile} | sed 's/\.c/.h/'`
      touch "${hfile}"
    done
    for file in `ls -1 *.c | sort | grep -v '^has'`; do
      (
        gcc -MM "${file}"
        echo "	\$(CC) \$(CFLAGS) \$(CPPFLAGS) -c ${file}"
        echo
      )
    done
    for cfile in `ls -1 has*.c | sort`; do
      hfile=`echo ${cfile} | sed 's/\.c/.h/'`
      rm -f "${hfile}"
    done

    for file in `ls -1 *.c | sort | grep -v '^has'`; do
      if grep '^int main(' "${file}" >/dev/null; then
        x=`echo "${file}" | sed 's/\.c$//'`
        echo "${x}: ${x}.o \$(OBJECTS) libs"
        echo "	\$(CC) \$(CFLAGS) \$(CPPFLAGS) -o ${x} ${x}.o \\"
        echo "	\$(OBJECTS) \$(LDFLAGS) \`cat libs\`"
        echo 
      fi
    done
    echo

    for cfile in `ls -1 has*.c | sort`; do
      hfile=`echo ${cfile} | sed 's/\.c/.h/'`
      lfile=`echo ${cfile} | sed 's/\.c/.log/'`
      touch "${hfile}"
      echo "${hfile}: tryfeature.sh ${cfile} libs"
      echo "	env CC=\"\$(CC)\" CFLAGS=\"\$(CFLAGS)\" LDFLAGS=\"\$(LDFLAGS) \`cat libs\`\" \\"
      echo "	./tryfeature.sh ${cfile} >${hfile} 2>${lfile}"
      echo "	cat ${hfile}"
      echo
    done

    echo "libs: trylibs.sh"
    echo "	env CC=\"\$(CC)\" ./trylibs.sh -lsocket -lnsl -lutil -lrandombytes -l25519 -lntruprime >libs 2>libs.log"
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

    echo "install: \$(BINARIES) \$(LINKS)"
    echo "	mkdir -p \$(DESTDIR)\$(PREFIX)/sbin"
    echo "	mkdir -p \$(DESTDIR)\$(PREFIX)/share/man/man8"
    echo "	\$(INSTALL) -m 0755 tinysshd \$(DESTDIR)\$(PREFIX)/sbin/tinysshd"
    echo "	\$(INSTALL) -m 0755 tinysshd-makekey \$(DESTDIR)\$(PREFIX)/sbin/tinysshd-makekey"
    echo "	\$(INSTALL) -m 0755 tinysshd-printkey \$(DESTDIR)\$(PREFIX)/sbin/tinysshd-printkey"
    echo "	\$(INSTALL) -m 0755 tinysshnoneauthd \$(DESTDIR)\$(PREFIX)/sbin/tinysshnoneauthd"
    echo "	\$(INSTALL) -m 0644 man/tinysshd.8 \$(DESTDIR)\$(PREFIX)/share/man/man8/tinysshd.8"
    echo "	\$(INSTALL) -m 0644 man/tinysshd-makekey.8 \$(DESTDIR)\$(PREFIX)/share/man/man8/tinysshd-makekey.8"
    echo "	\$(INSTALL) -m 0644 man/tinysshd-printkey.8 \$(DESTDIR)\$(PREFIX)/share/man/man8/tinysshd-printkey.8"
    echo "	\$(INSTALL) -m 0644 man/tinysshnoneauthd.8 \$(DESTDIR)\$(PREFIX)/share/man/man8/tinysshnoneauthd.8"
    echo

    echo "test: \$(BINARIES) \$(LINKS)"
    echo "	sh runtest.sh test-tinysshd.sh test-tinysshd.out test-tinysshd.exp"
    echo "	sh runtest.sh test-tinysshd-makekey.sh test-tinysshd-makekey.out test-tinysshd-makekey.exp"
    echo "	sh runtest.sh test-tinysshd-printkey.sh test-tinysshd-printkey.out test-tinysshd-printkey.exp"
    echo "	sh runtest.sh test-tinysshnoneauthd.sh test-tinysshnoneauthd.out test-tinysshnoneauthd.exp"
    echo "	sh runtest.sh test-crypto.sh test-crypto.out test-crypto.exp"
    echo

    echo "valgrindtest: \$(BINARIES) \$(LINKS)"
    echo "	sh runtest.sh valgrindtest-crypto.sh valgrindtest-crypto.out valgrindtest-crypto.exp"
    echo

    echo "clean:"
    echo "	rm -f *.out *.log libs \$(OBJECTS) \$(BINOBJ) \$(BINARIES) \$(LINKS) \$(AUTOHEADERS)"
    echo 

  ) > Makefile
)
