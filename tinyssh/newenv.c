/*
20140215
Jan Mojzis
Public domain.

The 'newenv' library is used to execute process in completely new environment.
*/

#include <unistd.h>
#include "e.h"
#include "byte.h"
#include "str.h"
#include "purge.h"
#include "bug.h"
#include "env.h"
#include "trymlock.h"
#include "newenv.h"

#define NEWENV_LEN 128
#define NEWENV_SPACE 2048

static struct newenv {
    char *e[NEWENV_LEN];
    long long elen;
    char es[NEWENV_SPACE];
    long long eslen;
} n = { {0}, 0, {0}, 0 };


/*
Remove sentitive data from allocated memory.
*/
void newenv_purge(void) {
    purge(&n, sizeof n);
    n.elen = NEWENV_LEN;
    n.eslen = NEWENV_SPACE;
    trymunlock(&n, sizeof n);
}

/*
Initialize newenv structure.
*/
void newenv_init(void) {
    trymlock(&n, sizeof n);
    purge(&n, sizeof n);
    n.e[0] = 0;
    n.elen = n.eslen = 0;
}

/*
The 'newenv_env/newenv_lowenv' function adds the variable
into new environment.
The newenv_lowenv is limited to a half of the buffer-space.
*/

int newenv_env_(int x, const char *s, const char *t) {

    long long slen, tlen, len, i;

    if (!x || !s || !t) bug_inval();
    if (n.elen < 0) bug_inval();
    if (n.eslen < 0) bug_inval();

    slen = str_len(s);
    tlen = str_len(t);
    len  = slen + tlen + 2;

    if (!n.elen) newenv_init();

    for (i = 0; n.e[i]; ++i) {
        if (str_start(n.e[i], s) && (n.e[i][slen] == '=')) {
            n.e[i] = n.e[--n.elen];
            break;
        }
    }

    if (n.elen + 2 > NEWENV_LEN / x) { errno = ENOMEM; return 0; }
    if (n.eslen + len > NEWENV_SPACE / x) { errno = ENOMEM; return 0; }

    byte_copy(n.es + n.eslen, slen, s); n.eslen += slen; n.es[n.eslen++] = '=';
    byte_copy(n.es + n.eslen, tlen, t); n.eslen += tlen; n.es[n.eslen++] = 0;
    n.e[n.elen++] = n.es + n.eslen - len;
    n.e[n.elen] = 0;
    return 1;
}

/*
The 'newenv_copyenv' function copies the variable
from current environment into new environment.
*/
int newenv_copyenv(const char *s) {

    char *t;

    if (!s) bug_inval();

    t = env_get(s);
    if (!t) { errno = ENOENT; return 0; }

    return newenv_env_(1, s, t);
}

/*
The 'newenv_exec' function executes the command
in new environment.
*/
void newenv_exec(char *filename, char **argv) {
    if (!argv || !filename) bug_inval();
    execve(filename, argv, n.e);
}
