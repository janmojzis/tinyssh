/*
20141207
Jan Mojzis
Public domain.
*/

#include "sshcrypto.h"
#include "writeall.h"
#include "global.h"

#define buf global_bspace1 /* reusing global buffer */

int main(void) {

    sshcrypto_random32(buf);

    if (writeall(1, buf, 32) == -1) global_die(111);
    global_die(0); return 111;
}
