compile: make-tinyssh.sh
	sh -e make-tinyssh.sh
clean:
	rm -rf build
install:
	sh -e make-install.sh $(DESTDIR)
