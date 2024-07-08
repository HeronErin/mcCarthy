#pragma once
#include <stdint.h>

typedef enum {
    // Size: 1 byte
    DT_BOOL,

    DT_BYTE,
    DT_UBYTE,

    // Size: 2 bytes
    DT_SHORT,
    DT_USHORT,

    DT_INT,    // 4 bytes

    DT_LONG,   // 8 bytes
    DT_FLOAT,  // 4 bytes
    DT_DOUBLE, // 8 bytes
    
    DT_VARINT,  // 1 <= n <= 5 bytes
    DT_VARLONG, // 1 <= n <= 10 bytes

    DT_STRING,     // Varint prefixed utf-8 string
    DT_IDENTIFIER, // Just a string 

    DT_TEXT_COMPONENT,
    DT_JSON_TEXT,

    DT_ENTITY_METADATA,
    DT_SLOT,
    DT_NBT,
    
    DT_POSITION,     // 8 bytes
    DT_ANGLE,        // 1 byte (1/256 of a turn, smart)
    DT_UUID,         // 16 bytes

    DT_BITSET,       // Length prefixed
    DT_BITSET_FIXED, // Of fixed size

    // Arrays
    DT_ARRAY,        // Containing felids 
    DT_BYTE_ARRAY,   // Length prefixed

} NetworkDatatype;

#include "datatypes.c"