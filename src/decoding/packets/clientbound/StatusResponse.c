#pragma once
#include "decoding/packet.h"
#include "decoding/datatypes.h"
#include <stdbool.h>


const PacketPrototype STATUS_RESPONSE_PROTO = {false, 0};

typedef struct{
    PacketPrototype packet;
    const char* data;
} StatusResponseS2C;




int encodeStatusResponseS2C(BUFF** buff, StatusResponseS2C* resultptr){
    if (
        0 != encodeVarInt(buff, resultptr->packet.packetId)
    ||  0 != encodeString(buff, resultptr->data, 32767)
    ) return -1;

    return 0;
}