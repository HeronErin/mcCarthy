#pragma once
#include "stdint.h"
#include <stddef.h>
#include <stdlib.h>

#define LENGTH(array) (sizeof(array)/sizeof(*(array)))
#define ARRAY_LENGTH(array) (sizeof((array))/sizeof((array)[0]))
#define SIZE(array, type) (sizeof(array) / (sizeof(type)))



typedef struct {
    size_t reserved;
    size_t size;
    size_t index;
    uint8_t data[];
} BUFF; 

BUFF* makeBuff(size_t size, size_t index);
BUFF* remakeBuff(BUFF* buff, size_t size, size_t index);

BUFF* quickBuff(size_t size, const uint8_t* data);
void writeByte(BUFF** buff, uint8_t b);

#define MK_BUFF(XXXX) quickBuff(sizeof(XXXX), (XXXX))

#include "common.c"