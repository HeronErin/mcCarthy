#pragma once
#include "datatypes.h"
#include "errno.h"
#include "stdint.h"
#include <stdint.h>

#include "common.h"
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
            if ((value & ~VARINT_SEGMENT) == 0) \
                return writeByte(buff, value); \
            if ( 0!=writeByte(buff, (value & VARINT_SEGMENT) | VARINT_CONTINUE_BIT) ) \
                return -1;\
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


#define _DECODE_RANGE_CHECK(__BUFFER, __ADD) if (__BUFFER->index + __ADD >= __BUFFER->size){\
        errno = ERANGE;\
        return -1;\
    }

int decodeByte(BUFF* buff, int8_t* val){
    _DECODE_RANGE_CHECK(buff, 0);
    *val = buff->data[buff->index++];
    return 0;
}
int decodeByteUnsigned(BUFF* buff, uint8_t* val){return decodeByte(buff, (int8_t*)val);}

int decodeShortUnsigned(BUFF* buff, uint16_t* ret){
    _DECODE_RANGE_CHECK(buff, 1);
    // Minecraft uses big endian for network stuff, so we must convert it to the local endianness
    *ret = be16toh(*(uint16_t*)(buff->data + buff->index));
    buff->index += 2;

    return 0;
}
int decodeShort(BUFF* buff, int16_t* ret) {return decodeShortUnsigned(buff, (uint16_t*) ret);}

int encodeShortUnsigned(BUFF** buff, uint16_t val){
    if (0 != extendFor(buff, (*buff)->index + 2))
        return -1;
    BUFF* b = *buff;
    *(uint16_t*)(b->data + b->index) = htobe16(val);
    b->index += 2;
    return 0;
}
int encodeShort(BUFF** buff, int16_t val){return encodeShortUnsigned(buff, val);}


int decodeIntUnsigned(BUFF* buff, uint32_t* ret){
    _DECODE_RANGE_CHECK(buff, 3);

    *ret = be32toh(*(uint32_t*)(buff->data + buff->index));
    buff->index += 4;

    return 0;
}
int decodeInt(BUFF* buff, int32_t* ret) {return decodeIntUnsigned(buff, (uint32_t*) ret);}

int encodeIntUnsigned(BUFF** buff, uint32_t val){
    if (0 != extendFor(buff, (*buff)->index + 4))
        return -1;
    BUFF* b = *buff;
    *(uint32_t*)(b->data + b->index) = htobe32(val);
    b->index += 4;
    return 0;
}
int encodeInt(BUFF** buff, int32_t val){return encodeIntUnsigned(buff, val);}


int decodeLongUnsigned(BUFF* buff, uint64_t* ret){
    _DECODE_RANGE_CHECK(buff, 7);

    *ret = be64toh(*(uint64_t*)(buff->data + buff->index));
    buff->index += 8;

    return 0;
}
int decodeLong(BUFF* buff, uint64_t* ret) {return decodeLongUnsigned(buff, (uint64_t*) ret);}

int encodeLongUnsigned(BUFF** buff, uint64_t val){
    if (0 != extendFor(buff, (*buff)->index + 8))
        return -1;
    BUFF* b = *buff;
    *(uint64_t*)(b->data + b->index) = htobe64(val);
    b->index += 8;
    return 0;
}
int encodeLong(BUFF** buff, int64_t val){return encodeLongUnsigned(buff, val);}

int decodeUUID(BUFF* buff, UUID* uuid){
    if (0 != decodeLongUnsigned(buff, &uuid->mostSignificant))
        return -1;
    if (0 != decodeLongUnsigned(buff, &uuid->leastSignificant))
        return -1;
    return 0;
}
int encodeUUID(BUFF** buff, UUID uuid){
    if (0 != encodeLongUnsigned(buff, uuid.mostSignificant))
        return -1;
    if (0 != encodeLongUnsigned(buff, uuid.leastSignificant))
        return -1;
    return 0;
}