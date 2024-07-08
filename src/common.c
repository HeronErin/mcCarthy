#include "common.h"
#include "stdint.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

BUFF* makeBuff(size_t size, size_t index){
    BUFF* buff = (BUFF*) malloc(size + sizeof(BUFF));
    buff->index = index;
    buff->size = size;
    buff->reserved = size;
    return buff;
}
BUFF* remakeBuff(BUFF* buff, size_t size, size_t index){
    if (buff->reserved > size){
        buff->size = size;
        return buff;
    }
    buff = (BUFF*) realloc(buff, size + sizeof(BUFF));
    buff->index = index;
    buff->size = size;
    buff->reserved = size;
    return buff;
}
void writeByte(BUFF** buff, uint8_t b){
    BUFF* bf = *buff;
    if (bf->index+1 >= bf->reserved){
        bf->reserved = (bf->index+1)*2; // This is a bad way of doing things
        bf = (BUFF*) realloc(bf, bf->reserved + sizeof(BUFF));
        *buff = bf;
    }
    bf->data[bf->index++] = b;
    if (bf->index > bf->size)
        bf->size = bf->index;
}

BUFF* quickBuff(size_t size, const uint8_t* data){
    BUFF* buff = makeBuff(size, 0);
    memcpy(buff->data, data, size);
    return buff;
}
