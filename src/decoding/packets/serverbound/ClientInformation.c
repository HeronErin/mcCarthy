#pragma once
#include "decoding/packet.h"
#include "decoding/datatypes.h"


typedef struct{
    PacketPrototype packet;
    uint8_t locale[STRING_LEN(16)];
    int8_t viewDistance;
    int chatMode; // 0: enabled, 1: commands only, 2: hidden
    bool chatColors;
    uint8_t skinMask;
    int isRightHand; // 0: left, 1: right
    bool enableTextFiltering;
    bool addPlayerToTab;
    
} ClientInformationPacketC2S;


int decodeClientInformationPacketC2S(uint8_t packetId, BUFF* buff, ClientInformationPacketC2S** resultptr){
    *resultptr = malloc(sizeof(ClientInformationPacketC2S));
    ClientInformationPacketC2S* result = *resultptr; 
    result->packet.packetId = packetId;
    result->packet.isServerBound = true;
    if (
        0 != decodeFixedString(buff, result->locale, STRING_LEN(16))
    ||  0 != decodeByte(buff, &result->viewDistance)
    ||  0 != decodeVarInt(buff, &result->chatMode)
    ||  0 != decodeByteUnsigned(buff, &result->skinMask)
    ||  0 != decodeVarInt(buff, &result->isRightHand)
    ||  0 != decodeByteUnsigned(buff, (uint8_t*) &result->enableTextFiltering)
    ||  0 != decodeByteUnsigned(buff, (uint8_t*) &result->addPlayerToTab)
    ) return -1;

    return 0;
}