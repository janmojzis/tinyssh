#include <stdio.h>
#define TRYINT(bits,target) \
static int tryint(void) \
{ \
  target x; \
  int i; \
 \
  x = 1; \
  for (i = 0;i < bits;++i) { \
    if (x == 0) return 100; \
    x += x; \
  } \
  if (x != 0) return 100; \
  x -= 1; \
  if (x > 0) return 100; \
 \
  return 0; \
}

#define DOINT(bits, target, typedefstr) \
TRYINT(bits,target) \
int main(void){ \
  if (tryint() != 0) return 100; \
  printf("#ifndef crypto_int%d_h\n", bits); \
  printf("#define crypto_int%d_h\n\n", bits); \
  printf("typedef %s crypto_int%d;\n\n", typedefstr, bits); \
  printf("#endif\n"); \
  return 0; \
}


#define TRYUINT(bits,target) \
static int tryuint(void) \
{ \
  target x; \
  int i; \
 \
  x = 1; \
  for (i = 0;i < bits;++i) { \
    if (x == 0) return 100; \
    x += x; \
  } \
  if (x != 0) return 100; \
  x -= 1; \
  if (x < 0) return 100; \
 \
  return 0; \
}

#define DOUINT(bits, target, typedefstr) \
TRYUINT(bits,target) \
int main(void){ \
  if (tryuint() != 0) return 100; \
  printf("#ifndef crypto_uint%d_h\n", bits); \
  printf("#define crypto_uint%d_h\n\n", bits); \
  printf("typedef %s crypto_uint%d;\n\n", typedefstr, bits); \
  printf("#endif\n"); \
  return 0; \
}
