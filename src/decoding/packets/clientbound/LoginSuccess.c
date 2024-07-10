#pragma once

#include "common.h"
#include "decoding/packet.h"
#include "decoding/datatypes.h"

const PacketPrototype LOGIN_SUCCESS_PROTO = {false, 0x02};


typedef struct{
    const char* name;
    const char* value;
    
    bool isSigned;
    const char* signature;

} PropertyListItem;

typedef struct{
    PacketPrototype packet;
    uint8_t username[STRING_LEN(16)];
    UUID uuid;

    int propertiesLength;
    PropertyListItem* properties;
    
    bool strictErrorHandling;
} LoginSuccessS2C;


int encodeLoginSuccessS2C(BUFF** buff, LoginSuccessS2C* resultptr){
    if (
        0 != encodeVarInt(buff, resultptr->packet.packetId)
    ||  0 != encodeUUID(buff, resultptr->uuid)
    ||  0 != encodeString(buff, resultptr->username, STRING_LEN(16))
    ||  0 != encodeVarInt(buff, resultptr->propertiesLength)
    ) return -1;
    int propl = resultptr->propertiesLength;
    if (propl != 0 && resultptr->properties == NULL){
        NULL_ERROR:
        errno = ENODATA;
        return -1;
    }
    for (int i = 0; i < propl; i++){
        PropertyListItem* prop = &resultptr->properties[i];
        if (
            0 != encodeString(buff, prop->name, STRING_LEN(32767))
        ||  0 != encodeString(buff, prop->value, STRING_LEN(32767))
        ||  0 != writeByte(buff, prop->isSigned)
        ) return -1;
        if (!prop->isSigned) continue;
        if (prop->signature == NULL)
            goto NULL_ERROR;
        if ( 0 != encodeString(buff, prop->signature, STRING_LEN(32767)))
            return -1;
    }
    if (0 != writeByte(buff, resultptr->strictErrorHandling))
        return -1;

    return 0;
}
