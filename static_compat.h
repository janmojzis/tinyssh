#ifndef STATIC_COMPAT_H____
#define STATIC_COMPAT_H____

#include <sys/types.h>
#include <pwd.h>

struct passwd *static_compat_getpwnam(const char *);
struct passwd *static_compat_getpwuid(uid_t);
int static_compat_initgroups(const char *, gid_t);

#ifdef STATIC_BUILD
#define tinyssh_getpwnam static_compat_getpwnam
#define tinyssh_getpwuid static_compat_getpwuid
#define tinyssh_initgroups static_compat_initgroups
#else
#define tinyssh_getpwnam getpwnam
#define tinyssh_getpwuid getpwuid
#define tinyssh_initgroups initgroups
#endif

#endif
