/* Public domain. */
extern int login_tty(int);

static void foo(void) { login_tty(0); }

int main(int argc, char **argv) {
    if (!argc) foo();
    return 0;
}
