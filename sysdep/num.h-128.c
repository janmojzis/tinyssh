/* Public domain. */
#include <stdio.h>
#include "trynum.h"

typedef int __attribute__((__mode__(__TI__))) num;
typedef unsigned int __attribute__((__mode__(__TI__))) num_u;

TRYINT(128, num)
TRYUINT(128, num_u)

int main(void) {
    
    if (tryint() != 0) return 111;
    if (tryuint() != 0) return 111;

    printf("#ifndef _NUM_H____\n");
    printf("#define _NUM_H____\n\n");
    printf("typedef int __attribute__((__mode__(__TI__))) num;\n");
    printf("typedef unsigned int __attribute__((__mode__(__TI__))) num_u;\n\n");
    printf("#endif\n");
    return 0;
}
