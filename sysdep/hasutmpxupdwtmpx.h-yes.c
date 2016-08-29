/* Public domain. */
#include <utmpx.h>

void foo(const char *wtmpx_file, const struct utmpx *utx);
void foo(const char *wtmpx_file, const struct utmpx *utx) {
    updwtmpx(wtmpx_file, utx);
}
