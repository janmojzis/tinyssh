#include "str.h"
#include "env.h"

/*
The 'env_get(name)' function returns the value of the first environment
variable whose name is 'name', or 0 if there is no such variable.
*/
char *env_get(const char *name) {

    long long len, i;

    if (!name) return 0;
    len = str_len(name);
    for (i = 0; environ[i]; ++i)
        if (str_start(environ[i], name) && (environ[i][len] == '='))
            return environ[i] + len + 1;
    return 0;
}
