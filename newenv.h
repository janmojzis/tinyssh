#ifndef _NEWENV_H____
#define _NEWENV_H____

extern int newenv_env_(int, const char *, const char *);

#define newenv_env(x, y) newenv_env_(1, (x), (y))
#define newenv_lowenv(x, y) newenv_env_(2, (x), (y))
extern int newenv_copyenv(const char *);
extern void newenv_exec(char *, char **);
extern void newenv_init(void);
extern void newenv_purge(void);

#endif
