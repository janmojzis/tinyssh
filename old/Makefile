compile: make-tinyssh.sh
	sh -e make-tinyssh.sh
cross-compile: make-tinysshcc.sh
	sh -e make-tinysshcc.sh
clean:
	rm -rf build
install:
	sh -e make-install.sh $(DESTDIR)
deb:
	dpkg-buildpackage -b -rfakeroot -us -uc
