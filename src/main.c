#include <stdint.h>
#include <stdio.h>
#include "common.h"
#include <stdlib.h>
#include <string.h>
#include "decoding/datatypes.h"

int main(){
    BUFF* b = makeBuff(32, 0);
    memcpy(b->data, (uint8_t[]){0x80, 1}, 2);
    printf("We got: %d\n", decodeVarint(b));
    
    return 0;
}