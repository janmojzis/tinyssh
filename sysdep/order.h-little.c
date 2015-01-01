/* Public domain. */
#include <stdio.h>

int main(void) {

    unsigned long long x = 1;
    unsigned char *l = (unsigned char *)&x;
    if (l[0] != 1) return 111;
    if (l[1] != 0) return 111;

    printf("#ifndef ORDER_LITTLEENDIAN\n");
    printf("#define ORDER_LITTLEENDIAN 1\n");
    printf("#endif\n");
    return 0;
}
