#pragma once
#include "datatypes.h"
#include "errno.h"
#include "stdint.h"
#include <stdint.h>

#include "../common.h"


#define VARINT_SEGMENT      0x7F
#define VARINT_CONTINUE_BIT 0x80

#define DEF_VAR_DECODE(FUNC_NAME, TYPE, BITSIZE) \
    TYPE FUNC_NAME (BUFF* buff){\
        uint8_t* from = buff->index + buff->data;\
        const uint8_t* extent = buff->size + buff->data;\
        TYPE value = 0;\
        int position = 0;\
        TYPE currentByte;\
        do{\
            if (extent <= from || position >= BITSIZE){\
                errno = position >= BITSIZE ? EOVERFLOW : ECHRNG;\
                return 0;\
            }\
            currentByte = *(from++);\
            value |= (currentByte & VARINT_SEGMENT) << position;\
            position += 7;\
        } while(currentByte & VARINT_CONTINUE_BIT);\
        buff->index = from - buff->data;\
        return value;\
    }
DEF_VAR_DECODE(decodeVarInt, int32_t, 32);
DEF_VAR_DECODE(decodeVarIntUnsigned, uint32_t, 32);
DEF_VAR_DECODE(decodeVarLong, int64_t, 64);
DEF_VAR_DECODE(decodeVarLongUnsigned, uint64_t, 64);