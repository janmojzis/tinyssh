/* Public domain. */

extern void logwtmp(const char *, const char *, const char *);

static void foo(void) { logwtmp("", "", ""); }

int main(int argc, char **argv) {
    if (!argc) foo();
    return 0;
}
