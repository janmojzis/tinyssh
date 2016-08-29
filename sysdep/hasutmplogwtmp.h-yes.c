/* Public domain. */

extern void logwtmp(const char *, const char *, const char *);

void foo(const char *line, const char *name, const char *host);
void foo(const char *line, const char *name, const char *host) {
    logwtmp(line, name, host);
}
