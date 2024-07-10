#pragma once
#include "decoding/packet.h"
#include "decoding/datatypes.h"


typedef struct{
    PacketPrototype packet;
    uint8_t username[STRING_LEN(16)];
    UUID uuid;
} LoginStartPacketC2S;


int decodeLoginStartPacketC2S(uint8_t packetId, BUFF* buff, LoginStartPacketC2S** resultptr){
    *resultptr = malloc(sizeof(LoginStartPacketC2S));
    LoginStartPacketC2S* result = *resultptr; 
    result->packet.packetId = packetId;
    result->packet.isServerBound = true;
    if (
        0 != decodeFixedString(buff, result->username, STRING_LEN(16))
    ||  0 != decodeUUID(buff, &result->uuid)
    ) return -1;

    return 0;
}