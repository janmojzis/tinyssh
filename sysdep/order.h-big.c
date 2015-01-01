/* Public domain. */
#include <stdio.h>

int main(void) {

    unsigned long long x = 1;
    unsigned char *l = (unsigned char *)&x;
    if (l[0] != 0) return 111;
    if (l[1] != 0) return 111;

    printf("#ifndef ORDER_BIGENDIAN\n");
    printf("#define ORDER_BIGENDIAN 1\n");
    printf("#endif\n");
    return 0;
}
