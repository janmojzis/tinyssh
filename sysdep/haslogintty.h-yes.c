/* Public domain. */
extern int login_tty(int);

void foo(int fd);
void foo(int fd) {
    login_tty(fd);
}
