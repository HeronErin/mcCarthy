#pragma once
#include "datatypes.h"
#include "errno.h"
#include "stdint.h"
#include <stdint.h>

#include "../common.h"
#include <endian.h>


#define VARINT_SEGMENT      0x7F
#define VARINT_CONTINUE_BIT 0x80



// ===========================================================
//           UNIVERSAL VARINT DECODER MACRO FUNCTION
// ===========================================================


#define DEF_VAR_DECODE(FUNC_NAME, TYPE, BITSIZE) \
    int FUNC_NAME (BUFF* buff, TYPE* value){\
        uint8_t* from = buff->index + buff->data;\
        const uint8_t* extent = buff->size + buff->data;\
        *value = 0;\
        int position = 0;\
        TYPE currentByte;\
        do{\
            if (extent <= from || position >= BITSIZE){\
                errno = position >= BITSIZE ? EOVERFLOW : ERANGE;\
                return -1;\
            }\
            currentByte = *(from++);\
            *value |= (currentByte & VARINT_SEGMENT) << position;\
            position += 7;\
        } while(currentByte & VARINT_CONTINUE_BIT);\
        buff->index = from - buff->data;\
        return 0;\
    }
DEF_VAR_DECODE(decodeVarInt, int32_t, 32);
DEF_VAR_DECODE(decodeVarIntUnsigned, uint32_t, 32);
DEF_VAR_DECODE(decodeVarLong, int64_t, 64);
DEF_VAR_DECODE(decodeVarLongUnsigned, uint64_t, 64);

// ===========================================================
//           UNIVERSAL VARINT ENCODER MACRO FUNCTION
// ===========================================================

#define DEF_VAR_WRITE(FUNC_NAME, TYPE) \
    int FUNC_NAME (BUFF** buff, TYPE value){ \
        while (1) { \
            if ((value & ~VARINT_SEGMENT) == 0) { \
                writeByte(buff, value); \
                return 0; \
            } \
            writeByte(buff, (value & VARINT_SEGMENT) | VARINT_CONTINUE_BIT);\
            value >>= 7;\
        }\
    }
DEF_VAR_WRITE(encodeVarIntUnsigned, uint32_t);
DEF_VAR_WRITE(encodeVarLongUnsigned, uint64_t);

int encodeVarInt(BUFF** buff, int32_t value){ return encodeVarIntUnsigned(buff, value); }
int encodeVarLong(BUFF** buff, int32_t value){ return encodeVarLongUnsigned(buff, value); }

// ===========================================================
//                  Primitive datatypes
// ===========================================================


int decodeByte(BUFF* buff, int8_t* val){
    if (buff->index >= buff->size){
        errno = ERANGE;
        return -1;
    }
    *val = buff->data[buff->index++];
    return 0;
}
int decodeByteUnsigned(BUFF* buff, uint8_t* val){return decodeByte(buff, (int8_t*)val);}

#define encodeByte writeByte
#define encodeByteUnsigned writeByte


int decodeShortUnsigned(BUFF* buff, uint16_t* ret){
    if (buff->index+1 >= buff->size){
        errno = ERANGE;
        return -1;
    }
    // Minecraft uses big endian for network stuff, so we must convert it to the local endianness
    *ret = be16toh(*(uint16_t*)(buff->data + buff->index));
    buff->index+=2;

    return 0;
}