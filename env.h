#ifndef _ENV_H____
#define _ENV_H____

/*
The environment 'environ', is a 0-terminated array
of 0-terminated strings, called environment variables.
Each environment variable is of the form name=value.
*/
extern char **environ;

extern char *env_get(const char *);

#endif
