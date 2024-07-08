#include <endian.h>
#include <stdint.h>
#include <stdio.h>
#include "common.h"
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"
#include "decoding/datatypes.h"

const uint8_t t[] = {};

int main(){
//     BUFF* b = MK_BUFF(t);
//     UUID x;
//     x.mostSignificant = 0;
//     x.leastSignificant = 21;
//     encodeUUID(&b, x);
//     b->index = 0;
//     UUID x2;
//     decodeUUID(b, &x2);
//     printf("We got %lu %lu\n", x2.mostSignificant, x2.leastSignificant);
    cJSON *json = cJSON_Parse("[1, 2, 3, 4]");
    printf("len: %x\n", json->type == cJSON_Array);

    cJSON_free(json);
    return 0;
}