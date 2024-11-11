#ifndef _GLOBAL_H____
#define _GLOBAL_H____

#define GLOBAL_BSIZE 65536
extern unsigned char global_bspace1[GLOBAL_BSIZE];
extern unsigned char global_bspace2[GLOBAL_BSIZE];

extern void global_init(void);
extern void global_purge(void);
extern void global_die(int);

#endif
