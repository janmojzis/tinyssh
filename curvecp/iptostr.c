#include "iptostr.h"

/* not thread-safe */

char *iptostr(unsigned char ip[4]) {

    long long i, j;
    static char strnum[20];
    unsigned long long num, num1, num2;

    j = 0;
    for(i = 0; i < 4; ++i) {
        num = ip[i];
        if (i != 0) strnum[j++] = '.';

        num1 = num / 100; num -= 100 * num1;
        num2 = num / 10; num -= 10 * num2;

        if (num1) {
             strnum[j++] = num1 + '0';
             strnum[j++] = num2 + '0';
        }
        else {
            if (num2) {
                strnum[j++] = num2 + '0';
            }
        }

        strnum[j++] = num + '0';
    }
    strnum[j] = 0;

    return strnum;
}
