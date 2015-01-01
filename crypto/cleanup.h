#ifndef _CLEANUP_H____
#define _CLEANUP_H____

#define cleanup(x) for (i = 0; i < sizeof(x); ++i) ((char *)x)[i] = 0;

#endif
