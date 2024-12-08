/* Public domain. */
#include <time.h>
#include <sys/time.h>
#include <utmp.h>

extern void login(const struct utmp *);
extern int logout(const char *);

static void foo(void) {
    struct utmp ut = {0};
    login(&ut);
    logout(ut.ut_line);
}

int main(int argc, char **argv) {
    if (!argc) foo();
    return 0;
}
