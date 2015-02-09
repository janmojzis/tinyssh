#include <signal.h>
#include <stdio.h>
#include <unistd.h>

#include "fep256.h"
#include "fe25519.h"


static int flagtimeout;
static void timeout(int x) {
    flagtimeout = x = 1;
    signal(SIGALRM, timeout);
}

int main(void) {

    long long count;
    fe z, x, y;

    signal(SIGALRM, timeout);

    /* fep256_mul test */
    alarm(1); flagtimeout = 0; count = 0;
    while (!flagtimeout) {
        fep256_mul(z, x, y);
        ++count;
    }
    printf("fep256_mul: %0.2f Mop/s\n", (double)count/1000000);


    /* fep256_sq test */
    alarm(1); flagtimeout = 0; count = 0;
    while (!flagtimeout) {
        fep256_sq(z, x);
        ++count;
    }
    printf("fep256_sq: %0.2f Mop/s\n", (double)count/1000000);

    /* fep256_mul2 test */
    alarm(1); flagtimeout = 0; count = 0;
    while (!flagtimeout) {
        fep256_mul2(z, x);
        ++count;
    }
    printf("fep256_mul2: %0.2f Mop/s\n", (double)count/1000000);

    /* fep256_mul3 test */
    alarm(1); flagtimeout = 0; count = 0;
    while (!flagtimeout) {
        fep256_mul3(z, x);
        ++count;
    }
    printf("fep256_mul3: %0.2f Mop/s\n", (double)count/1000000);


    /* fep256_mul6 test */
    alarm(1); flagtimeout = 0; count = 0;
    while (!flagtimeout) {
        fep256_mul6(z, x);
        ++count;
    }
    printf("fep256_mul6: %0.2f Mop/s\n", (double)count/1000000);


    /* fep256_add test */
    alarm(1); flagtimeout = 0; count = 0;
    while (!flagtimeout) {
        fep256_add(z, x, y);
        ++count;
    }
    printf("fep256_add: %0.2f Mop/s\n", (double)count/1000000);

    /* fep256_sub test */
    alarm(1); flagtimeout = 0; count = 0;
    while (!flagtimeout) {
        fep256_sub(z, x, y);
        ++count;
    }
    printf("fep256_sub: %0.2f Mop/s\n", (double)count/1000000);


    /* fe25519_mul test */
    alarm(1); flagtimeout = 0; count = 0;
    while (!flagtimeout) {
        fe25519_mul(z, x, y);
        ++count;
    }
    printf("fe25519_mul: %0.2f Mop/s\n", (double)count/1000000);


    /* fe25519_sq test */
    alarm(1); flagtimeout = 0; count = 0;
    while (!flagtimeout) {
        fe25519_sq(z, x);
        ++count;
    }
    printf("fe25519_sq: %0.2f Mop/s\n", (double)count/1000000);

    /* fe25519_mul121666 test */
    alarm(1); flagtimeout = 0; count = 0;
    while (!flagtimeout) {
        fe25519_mul121666(z, x);
        ++count;
    }
    printf("fe25519_mul121666: %0.2f Mop/s\n", (double)count/1000000);

    /* fe25519_add test */
    alarm(1); flagtimeout = 0; count = 0;
    while (!flagtimeout) {
        fe25519_add(z, x, y);
        ++count;
    }
    printf("fe25519_add: %0.2f Mop/s\n", (double)count/1000000);

    /* fe25519_sub test */
    alarm(1); flagtimeout = 0; count = 0;
    while (!flagtimeout) {
        fe25519_sub(z, x, y);
        ++count;
    }
    printf("fe25519_sub: %0.2f Mop/s\n", (double)count/1000000);



    return 0;
}
