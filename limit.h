#ifndef _LIMIT_H____
#define _LIMIT_H____

#include "haslimits.h"
#ifdef HASLIMITS
#include <limits.h>
#endif

#ifndef LOGIN_NAME_MAX
#define LOGIN_NAME_MAX 256
#endif

#ifndef NAME_MAX
#define NAME_MAX 255
#endif

#ifndef TTY_NAME_MAX
#define TTY_NAME_MAX 32
#endif

#endif
