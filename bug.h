#ifndef _BUG_H____
#define _BUG_H____

#include <unistd.h>
#include "global.h"
#include "e.h"
#include "log.h"

#define bug_(a, b) do { log_9_(-1, 0, a, b, 0, 0, 0, 0, 0, 0, 0, 0, 0); global_die(111); } while (0);
#define bug() bug_(__FILE__, __LINE__)

#define bug_nomem() do { errno = ENOMEM; bug(); } while (0);
#define bug_proto() do { errno = EPROTO; bug(); } while (0);
#define bug_inval() do { errno = EINVAL; bug(); } while (0);

#define bug_nomem_(a, b) do { errno = ENOMEM; bug_(a, b); } while (0);
#define bug_proto_(a, b) do { errno = EPROTO; bug_(a, b); } while (0);
#define bug_inval_(a, b) do { errno = EINVAL; bug_(a, b); } while (0);

#endif
