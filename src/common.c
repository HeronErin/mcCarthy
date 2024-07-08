#include "common.h"
#include "stdint.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

BUFF* makeBuff(size_t size, size_t index){
    BUFF* buff = (BUFF*) malloc(size + sizeof(BUFF));
    buff->index = index;
    buff->size = size;
    return buff;
}
BUFF* remakeBuff(BUFF* buff, size_t size, size_t index){
    buff = (BUFF*) realloc(buff, size + sizeof(BUFF));
    buff->index = index;
    buff->size = size;
    return buff;
}
BUFF* quickBuff(size_t size, const uint8_t* data){
    BUFF* buff = makeBuff(size, 0);
    memcpy(buff->data, data, size);
    return buff;
}