#ifndef _ORDER_H____
#define _ORDER_H____

#if defined(__amd64__) || defined(__x86_64__) || defined(__AMD64__) || defined(_M_X64) || defined(__amd64)
#define ORDER_LITTLEENDIAN
#endif

#if defined(__i386__) || defined(__x86__) || defined(__X86__) || defined(_M_IX86) || defined(__i386)
#define ORDER_LITTLEENDIAN
#endif

#if defined(__BYTE_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__)
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#undef ORDER_LITTLEENDIAN
#define ORDER_LITTLEENDIAN
#endif
#endif

#endif
