#include <endian.h>
#include <stdint.h>
#include <stdio.h>
#include "common.h"
#include <stdlib.h>
#include <string.h>
#include "decoding/datatypes.h"

const uint8_t t[] = {};

int main(){
    BUFF* b = MK_BUFF(t);
    UUID x;
    x.mostSignificant = 0;
    x.leastSignificant = 21;
    encodeUUID(&b, x);
    b->index = 0;
    UUID x2;
    decodeUUID(b, &x2);
    printf("We got %lu %lu\n", x2.mostSignificant, x2.leastSignificant);

    return 0;
}