#ifndef _PACKETPARSER_H____
#define _PACKETPARSER_H____

#include "crypto_uint32.h"
#include "crypto_uint8.h"

extern long long packetparser_uint8_(const char *, unsigned long long, const unsigned char *, long long, long long, crypto_uint8 *);
extern long long packetparser_uint32_(const char *, unsigned long long, const unsigned char *, long long, long long, crypto_uint32 *);
extern long long packetparser_copy_(const char *, unsigned long long, const unsigned char *, long long, long long, unsigned char *, long long);
extern long long packetparser_skip_(const char *, unsigned long long, const unsigned char *, long long, long long, long long);
extern long long packetparser_end_(const char *, unsigned long long, const unsigned char *, long long, long long);

#define packetparser_uint8(a, b, c, d)  packetparser_uint8_(__FILE__, __LINE__, (a), (b), (c), (d)) 
#define packetparser_uint32(a, b, c, d) packetparser_uint32_(__FILE__, __LINE__, (a), (b), (c), (d)) 
#define packetparser_copy(a, b, c, d, e)packetparser_copy_(__FILE__, __LINE__, (a), (b), (c), (d), (e)) 
#define packetparser_skip(a, b, c, d) packetparser_skip_(__FILE__, __LINE__, (a), (b), (c), (d)) 
#define packetparser_end(a, b, c) packetparser_end_(__FILE__, __LINE__, (a), (b), (c)) 

#endif

