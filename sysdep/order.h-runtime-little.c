#include <stdio.h>
#include <stdint.h>

union {
    uint32_t value;
    uint8_t data[4];
} number;

int main(void) {

    number.data[0] = 0x00;
    number.data[1] = 0x01;
    number.data[2] = 0x02;
    number.data[3] = 0x03;

    if (number.value == 0x03020100) {
        printf("/* Detected at run-time */\n");
        printf("#ifndef ORDER_LITTLEENDIAN\n");
        printf("#define ORDER_LITTLEENDIAN 1\n");
        printf("#endif\n");
        return 0;
    }
    return 111;
}
