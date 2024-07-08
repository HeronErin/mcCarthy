#include <endian.h>
#include <stdint.h>
#include <stdio.h>
#include "common.h"
#include <stdlib.h>
#include <string.h>
#include "decoding/datatypes.h"

const uint8_t t[] = {0, 0};

int main(){
    BUFF* b = MK_BUFF(t);
    uint16_t x = htobe16(69);
    memcpy(b->data, &x, 2);
    uint16_t x2;
    decodeShortUnsigned(b, &x2);
    
    printf("We got %hu  %d\n", x2, errno);

    return 0;
}