#ifndef _CHANNEL_H____
#define _CHANNEL_H____

#include "crypto_uint32.h"

#define CHANNEL_BUFSIZE 131072

/* channel_drop.c */
extern int channel_droppriv(char *, char **);
/* channel_fork.c */
extern long long channel_fork(int *);
/* channel_forkpty.c */
extern long long channel_forkpty(int *);

/* channel.c */
extern void channel_purge(void);
extern int channel_open(char *, crypto_uint32, crypto_uint32, crypto_uint32, crypto_uint32 *);
extern int channel_openterminal(char *, crypto_uint32, crypto_uint32, crypto_uint32, crypto_uint32);
extern void channel_ptyresize(crypto_uint32, crypto_uint32, crypto_uint32, crypto_uint32);
extern int channel_env(char *, char *);
extern int channel_exec(char *);
extern void channel_put(unsigned char *, long long);
extern void channel_puteof(void);
extern int channel_putisready(void);
extern long long channel_read(unsigned char *, long long);
extern long long channel_extendedread(unsigned char *, long long);
extern int channel_readisready(void);
extern int channel_extendedreadisready(void);
extern int channel_write(void);
extern int channel_writeisready(void);
extern int channel_iseof(void);
extern int channel_waitnohang(int *, int *);
extern void channel_purge(void);
extern void channel_init(void);

extern int channel_getfd0(void);
extern int channel_getfd1(void);
extern int channel_getfd2(void);
extern long long channel_getlen0(void);
extern crypto_uint32 channel_getid(void);
extern crypto_uint32 channel_getlocalwindow(void);
extern void channel_incrementremotewindow(crypto_uint32);
extern void channel_incrementlocalwindow(crypto_uint32);

/* channel subsystem.c */
#define CHANNEL_SUBSYSTEM_MAX 64
extern int channel_subsystem_add(const char *);
extern char *channel_subsystem_get(const char *);
extern void channel_subsystem_log(void);

#endif
