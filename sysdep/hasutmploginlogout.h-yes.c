/* Public domain. */
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <utmp.h>

extern void login(const struct utmp *);
extern int logout(const char *);

void fce(void) {

    struct utmp ut = {0};

    login(&ut);
    logout(ut.ut_line);
}

int main(void) {

    printf("#define HASUTMPLOGINLOGOUT 1\n");
    return 0;
}
